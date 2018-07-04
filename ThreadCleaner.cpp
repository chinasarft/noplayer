#include "ThreadCleaner.h"



ThreadCleaner::ThreadCleaner()
{
    auto clean = [this](){
        while(shouldQuit_ == false && threads_.size() == 0) {
            std::thread t;
            t.swap(threads_.front());
            threads_.pop_front();
            if (t.joinable()) {
                t.join();
            }
        }
    };

    cleaner_ = std::thread(clean);
}

void ThreadCleaner::Stop()
{
    shouldQuit_ = true;
    if (cleaner_.joinable()) {
        cleaner_.join();
    }
}

ThreadCleaner * ThreadCleaner::GetThreadCleaner()
{
    static ThreadCleaner cleaner;
    return &cleaner;
}

void ThreadCleaner::Push(std::thread && t)
{
    if (shouldQuit_) {
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    threads_.emplace_back(std::forward<std::thread>(t));
    condition_.notify_one();
}

