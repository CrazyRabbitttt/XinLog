#include "countDownLatch.h"

using namespace xinlog::detail;


CountDownLatch::CountDownLatch(int count)
    : m_mutex_(), m_cv_(), m_count_(count) {}

void CountDownLatch::wait() {
    // wait for count == 0
    std::unique_lock<std::mutex> lock(m_mutex_);
    while (m_count_ > 0) {
        m_cv_.wait(lock);
    }
}

void CountDownLatch::countDown() {
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
