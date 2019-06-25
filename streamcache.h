#ifndef STREAMCACHE_H
#define STREAMCACHE_H

#include <mutex>
#include <vector>
#include <deque>

class StreamCache
{
public:
    enum {
        OK = 0,
        TIMEOUT = -127,
        NODATA = -126,
        STOPPED = -125,
        UNKNOWN = -100
    };
public:
    StreamCache();
    int Append(const char *pData, int nDataLen);
    /*
     *  return <0 error
     *         >0 datalen
     */
    int Take(char *pData, int nDataLen, int nMilliSecTimeout);
    /*
     *  return <0 error
     *         OK success get one item
     */
    int TakeOne(std::vector<char> &item, int nMilliSecTimeout);
    void Stop();
    void Restart();

private:
    std::condition_variable condition_;
    std::mutex mutex_;
    std::deque<std::vector<char>> buffer_;
    int offset_ = 0;
    bool isStop_ = false;
};

#endif // STREAMCACHE_H
