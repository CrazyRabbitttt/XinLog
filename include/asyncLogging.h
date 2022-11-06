#ifndef XINLOG_ASYNCLOGGING_H_
#define XINLOG_ASYNCLOGGING_H_

#include "fixedBuffer.hpp"
#include "nocopyable.h"
#include "countDownLatch.h"

#include <memory>
#include <thread>
#include <atomic>
#include <vector>

namespace xinlog {
namespace detail {

class AsyncLogging : nocopyable{
 public:
    explicit AsyncLogging(std::string basename, off64_t rollSize, int flushInterval = 3);

    ~AsyncLogging();

    void append(const char* line, int len);

 private:
    void do_done();

    void thread_worker();
 private:
    using Buffer = FixedBuffer<kLargeBuffer>;
    using BufferPtr = std::unique_ptr<Buffer>;
    using BufferVectorPtr = std::vector<BufferPtr>;


   // 下面就是一些配置参数
    const int m_flushInterval;              // 3s刷盘一次
    const off64_t m_rollSize;               // rollSize 
    std::atomic<bool> m_done;               // atomic vari， if done 
    const std::string m_basename;   

    std::unique_ptr<std::thread> m_thread;  
    CountDownLatch latch_;   
    std::mutex mutex_;    
    std::condition_variable condv_;

    BufferPtr m_curBuffer;                 // double buffer ==> async write to disk 
    BufferPtr m_nextBuffer;
    BufferVectorPtr m_buffers;  
};  



}   // namespace detail   
}   // namespace xinlog 

#endif