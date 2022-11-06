#ifndef XINLOG_LOGGERUTIL_H
#define XINLOG_LOGGERUTIL_H

#include <ctime>
#include <string>

namespace xinlog {
    namespace detail {

        // 获得CurrentTime, ErrorInfo, LogFilename
        struct Util {
            static const char* getCurrentDataTime(bool isTime, time_t* now = nullptr);
            static const char* getErrorInfo(int error_code);
            static std::string getLogFileName(const std::string& basename, time_t& now);
        };
    }   // namespace xinlog::detail 
}   // namespace xinlog 

#endif