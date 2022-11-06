
#include "asyncLogging.h"

#include <string>

#include "loggerUtil.h"
#include "logFile.h"

using namespace xinlog;
using namespace xinlog::detail;



class LogFile;

AsyncLogging::AsyncLogging(std::string basename, off64_t rollSize, int flushInterval = 3)
    : m_basename(std::move(basename)), 
      m_rollSize(rollSize),
      m_flushInterval(flushInterval),
      m_done(false),
      latch_(1) 
{
    // 异常安全处理。。。。
    try {
        m_curBuffer  = std::make_unique<Buffer>();
        m_nextBuffer = std::make_unique<Buffer>();

        m_thread = std::make_unique<std::thread>([this]() {
            thread_worker();
        });
        // 需要等待线程的一些资源的初始化完成
        latch_.wait();
    } catch (...) {
        do_done();
        throw std::runtime_error("AsyncLogging create thread or buffer alloc error");
    }
}

AsyncLogging::~AsyncLogging() {
    do_done();
}


// 发生异常，需要维护最后的资源安全的退出
void AsyncLogging::do_done() {
    // 原子变量的标记为设置为 done
    m_done = true;

    condv_.notify_one();

    if (m_thread && m_thread->joinable()) {
        // 等待线程处理完剩余的资源
        m_thread->join();
    }
}




// async append 操作
void AsyncLogging::append(const char*line, int len) {
    /*
        双缓冲机制(curbuffer, nextbuffer), 如果说curbuffer有空间那么就push，没有就写到vector队列中
        备用buffer, 使用 std::move() 能够进行数据所有权的转移而不是拷贝数据
    */

   std::lock_guard<std::mutex> lock(mutex_);

    // 直接append到curbuffer就好了
    if (m_curBuffer->avail() > len) {
        m_curBuffer->append(line, len);
        return; 
    }

    // 缓存满了 需要进行flush操作
    m_buffers.push_back(std::move(m_curBuffer));    // 转移指针的所有权
    if (m_nextBuffer) {
        // 将备用的给替换上来
        m_curBuffer = std::move(m_nextBuffer);
    } else {        // 没有备用的那就要重新申请了
        m_curBuffer = std::make_unique<Buffer>();
    }

    m_curBuffer->append(line, len);
    condv_.notify_one();            // 通知进行消费
}



// 异步的写入（消费者：从vector中进行消费） 
void AsyncLogging::thread_worker() {
    try {

        BufferPtr newBuffer1 = std::make_unique<Buffer>();
        BufferPtr newBuffer2 = std::make_unique<Buffer>();
        newBuffer1->bzero();
        newBuffer2->bzero();

        BufferVectorPtr buffersToWrite;     // 帮助m-buffers写入磁盘
        buffersToWrite.reserve(16);         // 预分配数据

        static bool once = false;

        while (!m_done) {
            // 缩减临界区，因为是操作的私有对象 buffersToWrite 
            {
                std::unique_lock<std::mutex> lock(mutex_);

                if (m_buffers.empty()) {
                    if (!once) {
                        once = true;
                        latch_.countDown();            // thread 的资源初始化完成了，通知主线程
                    }   
                    // 每3秒进行一次刷盘操作
                    condv_.wait_for(lock, std::chrono::seconds(m_flushInterval));
                }
                m_buffers.push_back(std::move(m_curBuffer));    // 不管怎样都将 curBuffer 刷盘
                m_curBuffer = std::move(newBuffer1);            // 换上新的内存
                buffersToWrite.swap(m_buffers);
                if (!m_nextBuffer) {
                    m_nextBuffer = std::move(newBuffer2);       // 换上新的内存
                }
            }

            if (buffersToWrite.empty()) continue;

            if (buffersToWrite.size() > 25) {                   // 需要刷盘数据过多，保留2个
                char buf[256];
                snprintf(buf, sizeof(buf), "Dropped log messages at %s, %zd larger buffers\n",
                Util::getCurrentDataTime(true),
                buffersToWrite.size() - 2);
                fputs(buf, stderr);
                
            }


        }

    } catch (std::exception const& e) {

    }
}
