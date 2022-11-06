#ifndef XINLOG_FILEAPPENDER_H_
#define XINLOG_FILEAPPENDER_H_

#include "nocopyable.h"

#include <cstdio>
#include <string>


namespace xinlog {
namespace detail {

class FileAppender : nocopyable{
 public:
    explicit FileAppender(const std::string& filename);

    ~FileAppender();
    void append(const char* line, size_t len);
    void flush();

    off64_t writtenBytes() const { return writtenBytes_; }
    void resetWrittrn() { writtenBytes_ = 0; }
 private:
    size_t write(const char* line, size_t len);
    void init(std::string const &filename);

 private:
    FILE* file_{};    
    char buffer_[64*1024];                // 用不用这个 buffer 性能差距不是很大
    off64_t writtenBytes_;
};  


}   // namespace xinlog::detail 
}   // xinlog 

#endif