#ifndef XINLOG_LOGGER_CONFIG_H
#define XINLOG_LOGGER_CONFIG_H

#include "fmt/format.h"

#include <functional>
#include <cstdio>


namespace xinlog {

    enum PRINT_FLAG {
        Ldata = 1 << 0,
        Ltime = 1 << 1,
        Llongname  = 1 << 2,
        Lshortname = 1 << 3,
        Lline = 1 << 4,
        LthreadId = 1 << 5,
        LstdFlags = Ldata | Ltime | Lshortname | Lline
    };

    enum LEVEL {
        L_DEBUG_,
        L_INFO,
        L_WARN,
        L_ERROR,
        L_FATAL,
        LEVEL_COUNT
    };

    namespace detail {
        using callback_t = std::function<void(fmt::memory_buffer &)>;

        class Config {
         public:
            static Config& instance();      // get instance of config

          const char* &basename() {return m_log_basename;   }
            int &flags()          { return m_log_flags;     }
            bool &console()       { return m_log_console;   }
            int& rollSize()       { return m_log_rollSize;  }
            int& flushInterval()  { return m_log_flushInterval;}
            const char* &prefix() { return m_log_prefix;    }
            callback_t& before()  { return m_before;        }
            callback_t& end()     { return m_end;           }
         private:
            int m_log_flags = LstdFlags;
            bool m_log_console = true;
            int m_log_rollSize = 20 * 1024 * 1024;      // rollSize 为 20M
            int m_log_flushInterval = 3;                // 3秒刷新一次日志
            const char* m_log_basename = nullptr;
            const char* m_log_prefix   = nullptr;
            callback_t m_before;
            callback_t m_end;
        };


    }   // namespace detail 

}   // namespace xinlog 

#endif