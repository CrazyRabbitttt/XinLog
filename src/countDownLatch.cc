#include "countDownLatch.h"

using namespace xinlog::detail;


CountDownLatch::CountDownLatch(int count)
    : m_mutex_(), m_cv_(), m_count_(count) {}

void CountDownLatch::wait() {

    // wait 需要用 unique_ptr, wait() 能够进行解锁
    std::unique_lock<std::mutex> lock(m_mutex_);        
    while (m_count_ > 0) {
        m_cv_.wait(lock);
    }
}


void CountDownLatch::countDown() {
    // lock
    std::lock_guard<std::mutex> lock(m_mutex_);
    --m_count_;
    if (m_count_ == 0) {
        m_cv_.notify_all();
    }
}

int CountDownLatch::getCount() {
    std::lock_guard<std::mutex> lock(m_mutex_);
    return m_count_;
}
