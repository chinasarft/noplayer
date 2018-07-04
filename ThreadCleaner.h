#ifndef THREADCLEARNER_H
#define THREADCLEARNER_H

#include <queue>
#include <mutex>
#include <thread>


class ThreadCleaner
{
public:
    static ThreadCleaner * GetThreadCleaner();
    void Push(std::thread && t);
    void Stop();

private:
    ThreadCleaner();
    std::thread cleaner_;
    std::deque<std::thread> threads_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool shouldQuit_ = false;
};

#endif // THREADCLEARNER_H
