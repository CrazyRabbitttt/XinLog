#include "logFile.h"


#include "loggerUtil.h"
#include "fileAppender.h"

using namespace xinlog::detail;


LogFile::LogFile(std::string basename, off64_t rollSize, bool threadSafe, int flushInterval, int checkEveryN)
    : basename_(std::move(basename)),
      rollSize_(rollSize),
      flushInterval_(flushInterval),
      checkEveryN_(checkEveryN),
      mutex_(threadSafe ? std::make_unique<std::mutex>() : nullptr) {
            rollFile();
}


void LogFile::append(const char* line, int len) {
    if (mutex_) {
        std::lock_guard<std::mutex> lock(*mutex_);
        append_unlocked(line, len);
    } else {
        append_unlocked(line, len);
    }
}

void LogFile::flush() {
    // 线程安全就是需要进行加锁的操作
    if (mutex_) {
        std::lock_guard<std::mutex> lock(*mutex_);
        file_->flush();
    } else {
        file_->flush();
    }
}


void LogFile::append_unlocked(const char* line, int len) {
    file_->append(line, len);
    if (file_->writtenBytes() > rollSize_) {        // 单文件超出规定大小、滚动日志
        rollFile();
        file_->resetWrittrn();
    } else {
        ++count_;
        if (count_ >= checkEveryN_) {
            count_ = 0;
            time_t now = ::time(nullptr);
            time_t curPeriod = now / kRollPerSeconds * kRollPerSeconds;
            if (curPeriod != lastPeriod_) {
                rollFile(&now);
            } else if (now - lastFlush_ > flushInterval_) {
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

void LogFile::rollFile(const time_t* cache_now) {
    time_t now;
    if (cache_now != nullptr) {
        now = *cache_now;
    } else {
        now = time(nullptr);
    }

    auto filename = Util::getLogFileName(basename_, now);
    auto start = now / kRollPerSeconds * kRollPerSeconds; //更新天的数据

    // 超过了阈值就创建个新的
    if (now > lastRoll_) {

        lastRoll_ = now;
        lastFlush_ = now;
        lastPeriod_ = start;
        file_ = std::make_unique<FileAppender>(filename); //创建新的文件
    }
}



