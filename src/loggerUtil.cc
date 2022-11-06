#include "loggerUtil.h"

#include <cstdio>
#include <cassert>

#include "ProcessInfo.h"

using namespace xinlog::detail;

thread_local struct tm t_tm;                    // thread local, time各种数据的结构体
thread_local time_t t_lastSecond;               // thread local, 如果一秒的话直接复用
thread_local char t_time[64];

// 获得格式化之后的时间
const char* getCurrentDataTime(bool isTime, time_t* now = nullptr) {
    time_t timer = time(nullptr);
    if (now != nullptr) *now = timer;   // 减少 syscall 

    // 如果说时间和上一次一样（差距小于1s）， 那么直接用 thread_local member 就好了
    if (t_lastSecond != timer) {
        t_lastSecond = timer;
        localtime_r(&timer, &t_tm);
    }

    int len;

    if (isTime) {       // 将时间数据格式化到 char[] 中
        len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d",
                t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday,
                t_tm.tm_hour, t_tm.tm_min, t_tm.tm_sec);
        assert(len == 19);
    } else {
        len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d",
                       t_tm.tm_year + 1900, t_tm.tm_mon + 1, t_tm.tm_mday);
        assert(len == 10);
    }
    return t_time;
}

