#ifndef XINLOG_PROCESSINFO_H
#define XINLOG_PROCESSINFO_H

#include <sys/types.h>
#include <unistd.h>
#include <string>

namespace xinlog {
    namespace ProcessInfo {
        pid_t GetPid();
        pid_t GetTid();
        uid_t GetUid();
        std::string GetHostName();
    }   // namespace Process Info 
}   // namespace xinlog 

#endif