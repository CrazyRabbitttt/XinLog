#ifndef XINLOG_NOCOPYABLE_H_
#define XINLOG_NOCOPYABLE_H_


namespace xinlog {

    class nocopyable {
     public:
        nocopyable(const nocopyable&) = delete;
        void operator=(const nocopyable&) = delete;
     protected:
        nocopyable()  = default;
        ~nocopyable() = default;
    };
}   // namespace xinlog 

#endif

