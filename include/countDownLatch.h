#ifndef XINLOG_COUNTDOWNLATCH_H
#define XINLOG_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>


namespace xinlog {
 namespace detail {

 class CountDownLatch {
  public:
    explicit CountDownLatch(int count);

    void wait();
    void countDown();
    int getCount();
  private:
    std::condition_variable m_cv_;
    std::mutex m_mutex_;
    int m_count_;     
 };

 }  // namespace detail 
}   // namespace xinlog 

#endif