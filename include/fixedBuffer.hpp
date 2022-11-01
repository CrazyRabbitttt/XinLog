#ifndef XINLOG_FIXEDBUFFER_H_
#define XINLOG_FIXEDBUFFER_H_

#include "nocopyable.h"

#include <string.h>

namespace xinlog {

namespace detail {

enum { kSmallBuffer = 4096, kLargeBuffer = 4096 * 1024};

template <int SIZE>
class FixedBuffer : nocopyable {
 public:    
    FixedBuffer() : m_cur_buffer_(m_buffer_) {}

    int avail()  { return static_cast<int>(end() - m_cur_buffer_);       }
    int size()   { return static_cast<int>(m_cur_buffer_ - m_buffer_);   }
    char* data() { return m_buffer_; }
    void bzero() { memset(m_buffer_, 0, sizeof(m_buffer_));              }
    void reset() { m_cur_buffer_ = m_buffer_;                            }
    void append(const char* line, size_t len) {
        if (avail() > len) {
            memcpy(m_cur_buffer_, line, len);
            m_cur_buffer_ += len;
        }
    }

 private:
    char* end() { return m_buffer_ + sizeof(m_buffer_); }

    char m_buffer_[SIZE]{};     // fiex buffer
    char* m_cur_buffer_{};      // 目前写入的位置
};



}   // namespace detail 

}   // namespace xinlog 

#endif