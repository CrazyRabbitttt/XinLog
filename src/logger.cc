#include "logger.h"

#include <cstring>
#include <cerrno>

#include "logConfig.h"
#include "fmt/chrono.h"
#include "loggerUtil.h"
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

fmt::color s_color[LEVEL_COUNT + 1] = {
    fmt::color::blue,       fmt::color::green,  fmt::color::yellow,
    fmt::color::red,        fmt::color::purple,
    fmt::color::orange_red, // 如果不在所有的范围内
};


inline const char* GET_LEVEL_TEXT(int level) {
    if (level >= LEVEL_COUNT || level < 0)
        return s_level_text[LEVEL_COUNT];   // level 不在范围内不
    return s_level_text[level];
}

inline fmt::color GET_COLOR_BY_LEVEL(int level) {
    if (level >= LEVEL_COUNT || level < 0)
        return s_color[LEVEL_COUNT];
    return s_color[level];
}


detail::Logger::Logger() { init_data(); }

detail::Logger::~Logger() = default;

void detail::Logger::init_data() {
    // 进行logger 的初始化， 通过config判断是否输出到文件
    if (LOG_CONFIG.basename() != nullptr) {
        m_logging = std::make_unique<AsyncLogging> (
            LOG_CONFIG.basename(),
            LOG_CONFIG.rollSize(),
            LOG_CONFIG.flushInterval()
        );
    }
}

// void detail::Logger::internal_log(const detail::context& ctx) {
//     flockfile(stdout);
//     fmt::print(stdout, fg(fmt::color::green),
//                "[LOG] [pid:{}] [tid:{}] {:%Y-%m-%d -%H:%M:%S} {}:{:d} {}\r\n",
//                ProcessInfo::GetPid(), ProcessInfo::GetTid(),
//                fmt::localtime(time(nullptr)), ctx.long_filename, ctx.line,
//                ctx.text);
//     funlockfile(stdout);
// }

void detail::Logger::LogFile(const detail::context& ctx) {
    int tid         = ProcessInfo::GetTid();                // thread id 
    auto*leveltext  = GET_LEVEL_TEXT(ctx.level);            // level text 
    const char* filename = 
        LOG_CONFIG.flags() & Llongname ? ctx.log_filename  :
        ((LOG_CONFIG.flags() & Lshortname) ? ctx.short_filename : nullptr);
    
    // fmt 进行数据的写入，性能是比较好的
    fmt::memory_buffer buffer;

    // 年月日：时分秒
    if (LOG_CONFIG.flags() & Ldata) {
        fmt::format_to(std::back_inserter(buffer), "{} ", 
            Util::getCurrentDataTime(LOG_CONFIG.flags() & Ltime));
    }

    // 前缀, 可以自己设定前缀的哦
    if (LOG_CONFIG.prefix()) {
        fmt::format_to(std::back_inserter(buffer), "{} ", LOG_CONFIG.prefix());
    }

    if (LOG_CONFIG.before()) {
        LOG_CONFIG.before()(buffer);
    }

    // 日志的等级标记
    fmt::format_to(std::back_inserter(buffer), " {}", leveltext);

    // 线程ID
    if (LOG_CONFIG.flags() & LthreadId) {
        fmt::format_to(std::back_inserter(buffer), " [tid:{:d}] ", tid);
    }

    // filename
    if (filename != nullptr) {
        fmt::format_to(std::back_inserter(buffer), " [{}:{:d}] ", filename, ctx.line);
    }

    if (LOG_CONFIG.end()) {
        LOG_CONFIG.end()(buffer);
    }

    if (ctx.level == L_FATAL) { // 如果是fatal等级，则获取errno产生的错误
        fmt::format_to(std::back_inserter(buffer), " {}:{} \r\n", ctx.text,
                       Util::getErrorInfo(errno)); // 打印提示信息
    } else {
        fmt::format_to(std::back_inserter(buffer), " {} \r\n",
                       ctx.text); // 打印提示信息
    }

    // 将数据【buffer中的】写到Async缓冲区中
    m_logging->append(buffer.data(), static_cast<int>(buffer.size()));
}


// DoLog, passed the context
void detail::Logger::DoLog(detail::context const &ctx) {
    if (m_logging) {
        LogFile(ctx);
    }

    // 那么就直接输出到控制台

}



