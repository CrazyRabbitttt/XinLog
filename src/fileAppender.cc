
#include "fileAppender.h"

#include <string>
#include <stdexcept>

#include "loggerUtil.h"

using namespace xinlog::detail;


FileAppender::FileAppender(const std::string& filename) {
    init(filename);
}


void FileAppender::init(const std::string& filename) {
    // 设置一下 FILE 啥的

    std::string filedir;
    size_t pos;
    if ((pos = filename.rfind('/')) == std::string::npos) {
        // 如果说没有找到 '/'
        throw std::runtime_error("invalid filepath");
    }

    filedir = filename.substr(0, pos + 1);

    file_ = fopen(filename.c_str(), "ae");
    if (file_ == nullptr) {
        int err = ferror(file_);
        auto* errorInfo = Util::getErrorInfo(err);
        fprintf(stderr, "FileAppender error in open file:%s erron:%s", filename.c_str(), errorInfo);
        return ;
    }
    ::setbuffer(file_, buffer_, sizeof(buffer_));
}


FileAppender::~FileAppender() {
    if (file_ != nullptr) {
        ::fflush(file_);
        ::fclose(file_);
    }
}

void FileAppender::append(const char* line, size_t len) {
    size_t written = 0;
    while (written != len) {
        size_t remain = len - written;
        size_t n = write(line + written, remain);
        if (n != remain) {      // 没写完啊🤔
            int err = ferror(file_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", Util::getErrorInfo(err));
                break;
            }
            if (n == 0) { throw std::runtime_error("write 出错了,写入返回0"); }
        }
        written += n;
    }
    writtenBytes_ += written;
}

void FileAppender::flush() {
    if (file_) {
        ::fflush(file_);
    }
}


// 内部封装 unlocked write 
size_t FileAppender::write(const char* line, size_t len) {
    size_t n = 0;
    if (file_) {
        n = fwrite_unlocked(line, 1, len, file_);
    }
    return n;
}

