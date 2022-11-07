#ifndef XINLOG_LOGFILE_H_
#define XINLOG_LOGFILE_H_

#include "nocopyable.h"

#include <sys/types.h>
#include <mutex>
#include <memory>

#include "fileAppender.h"

namespace xinlog {
namespace detail {

// class FileAppender;

// 内部 同样也是封装了 fileAppend, 但是能够做到 rollFile 
class LogFile : nocopyable{
 public:    
    explicit LogFile(std::string basename, off64_t rollSize, bool threadSafe = true,
        int flushInterval = 3, int checkEveryN = 1024);
    
    ~LogFile() = default;

    void append(const char* line, int len);
    void flush();
    void rollFile(const time_t* now = nullptr);

 private:
    void append_unlocked(const char* line, int len);

 private:
    enum { kRollPerSeconds = 60 * 60 * 24 };

    const off64_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;     
    int count_;

    const std::string basename_;
    std::unique_ptr<std::mutex> mutex_;

    time_t lastPeriod_{};
    time_t lastRoll_{};
    time_t lastFlush_{};
    std::unique_ptr<FileAppender> file_;            // 真正的写入到文件系统的 appender 

};


}   // namespace xinlog::detail
}   // namespace xinlog 

#endif