#include "logger.h"

#include <cstring>
#include <cerrno>

#include "logConfig.h"
#include "fmt/chrono.h"
#include "processInfo.h"

using namespace xinlog;

#define LOG_CONFIG detail::Config::instance()

// 外面操作的接口 Config 其实掌控了内部的实际上的 Config
void xinlog::Config::Set(const xinlog::Config& config) {
    LOG_CONFIG.basename() = config.output_basedir;
    LOG_CONFIG.flags()    = config.print_flag;
    LOG_CONFIG.console()  = config.is_console;
    LOG_CONFIG.prefix()   = config.output_prefix;
    LOG_CONFIG.before()   = config.before;
    LOG_CONFIG.end()      = config.end;
}

// level to text 
const char* s_level_text[LEVEL_COUNT + 1] = {
    "[DEBUG]  ", "[INFO]   ",
    "[WARN]   ", "[ERROR]  ",
    "[FATAL]  ", "[UNKNOWN]"
};

// fmt::color s_color[LEVEL_COUNT + 1] = {
//     fmt::color::blue,       fmt::color::green,  fmt::color::yellow,
//     fmt::color::red,        fmt::color::purple,
//     fmt::color::orange_red, // 如果不在所有的范围内
// };


inline const char* GET_LEVEL_TEXT(int level) {
    if (level >= LEVEL_COUNT || level < 0)
        return s_level_text[LEVEL_COUNT];   // level 不在范围内不
    return s_level_text[level];
}


detail::Logger::Logger() { init_data(); }

void detail::Logger::init_data() {
    // 进行logger 的初始化， 通过config判断是否输出到文件
    if (LOG_CONFIG.basename() != nullptr) {

    }
}

void detail::Logger::LogFile(const detail::context& ctx) {
    int     tid     = ProcessInfo::GetTid();
    auto *level_text= GET_LEVEL_TEXT(ctx.level);
    const char* filename = 
        LOG_CONFIG.flags() & Llongname ? 
            ctx.log_filename : ((LOG_CONFIG.flags() & Lshortname) ?
                ctx.short_filename : nullptr);

    fmt::memory_buffer buffer;

    if (LOG_CONFIG.flags() & Ldata) {       // 打印年月日
        
    }


}



