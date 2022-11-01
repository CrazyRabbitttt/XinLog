#ifndef MY_LOGGER_H_
#define MY_LOGGER_H_

#include "Config.h"

#include <functional>
#include <string>
#include <cstring>



namespace xinlog {


#define DEBUG_
#define LOG_
#define WARN_
#define ERROR_
#define FATAL_

using std::string;

    // 配置信息
    struct Config {
        int print_flag = LstdFlags;
        const char* output_prefix  = nullptr;
        const char* output_basedir = nullptr;
        bool is_console = true; 

        detail::callback_t before;
        detail::callback_t end;

        static void Set(const Config& config); 
    };


    namespace detail {

        struct context;                     // to store the context from front end 

        class Logger {
         private:    
            Logger();

         public:
            ~Logger();

            static Logger& GetInstance();       // 单例模式获得Logger

            void DoLog(context const& ctx);
            void LogFile(context const& ctx);
            static void LogConsole(const context& ctx);
            static void internal_log(const context& msg);

         private:   
            void init_data();
            // std::unique
        };

        struct context {                    // 传输数据的上下文
            int level;
            int line;
            const char* short_filename{};
            const char* log_filename{};
            string text;
        };  

        // 外部调用接口， 最终使用获得的单例模式对象调用 member function 
        inline void DoLog(context& ctx) {
            detail::Logger::GetInstance().DoLog(ctx);   
        }

        inline const char* GetShortName(const char* filename) {
            int len = strlen(filename);
            int pos = 0;
            for (int i = len - 1; i >= 0; i--) {
                if (filename[i] == '/' || filename[i] == '\\') {
                    pos = i + 1;
                    break;
                }
            }
            return filename + pos;
        } 

    }   // namespace detial


#define XINLOG_NAMESPACE xinlog::detial::

#define INIT_LOG(level_)    \
    ctx.level = level_;     \
    ctx.line  = __LINE__;   \
    ctx.long_filename  = __FILE__;  \
    ctx.short_filename = XINLOG_NAMESPACE GetShortName(__FILE__)



/*
    获得 context 文本
    初始化行号，文件名，日志级别等信息
    内部调用单例对象的接口
*/

#ifdef DEBUG_
#define debug(fmt_, args_...) do {  \
        XINLOG_NAMESPACE context ctx;       \               
        ctx.text = fmt.format(fmt_,##args_); \
        INIT_LOG_(0);           \
        detail::DoLog(ctx);      \        
    } while (false)
#else
#define debug(format, args...)
#endif

#ifndef INFO_
#define info(fmt_, args_...) do {   \
        XINLOG_NAMESPACE context ctx;   \
        ctx.text = fmt.format(fmt_,##args_); \
        INIT_LOG_(1);   \
        detail::DoLog(ctx); \
    } while(false)
#else 
#define info(format, args...)
#endif




}   // xinlog 

#endif