#include "streamcache.h"
#include <cstring>
#include <fstream>
//#define OUT_PUT_TO_FILE

#ifdef OUT_PUT_TO_FILE
static std::ofstream recved;
static std::ofstream poped;
#endif

StreamCache::StreamCache()
{
#ifdef OUT_PUT_TO_FILE
    if (!recved.is_open()) {
        recved.open("recved.h264");
    }
    if (!poped.is_open()) {
        poped.open("poped.h264");
    }
#endif
}

void StreamCache::Stop() {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        isStop_ = true;
    }
    condition_.notify_one();
}

void StreamCache::Restart() {
    std::unique_lock<std::mutex> lock(mutex_);
    isStop_ = false;
}

int StreamCache::Append(const char *pData, int nDataLen) {
    assert(pData != nullptr && nDataLen != 0);

#ifdef OUT_PUT_TO_FILE
    if (recved.is_open()) {
        recved.write(pData, nDataLen);
    }
#endif
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (isStop_)
            return STOPPED;
        std::vector<char> item;
        item.resize(nDataLen);
        memcpy(item.data(), pData, nDataLen);
        buffer_.emplace_back(std::move(item));
    }
    condition_.notify_one();
    return OK;
}

int StreamCache::Take(char *pData, int nDataLen, int nMilliSecTimeout) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (isStop_)
        return STOPPED;
    if (buffer_.size() == 0) {
        if (nMilliSecTimeout > 0) {
            auto status = condition_.wait_for(lock, static_cast<std::chrono::milliseconds>(nMilliSecTimeout));
            if (status == std::cv_status::timeout) {
                return TIMEOUT;
            }
        } else {
            condition_.wait(lock);
        }
    }
    int nRemain = nDataLen;
    do {
        if (buffer_.size() == 0) {
#ifdef OUT_PUT_TO_FILE
            if (poped.is_open()) {
                poped.write(pData, nDataLen - nRemain);
            }
#endif
            offset_ = 0;
            return UNKNOWN;
        }
        auto item = buffer_.front();

        int itemLen = item.size() - offset_;
        int copyLen = nRemain <= itemLen ? nRemain : itemLen;
        memcpy(pData + nDataLen - nRemain, item.data() + offset_, copyLen);
        nRemain -= copyLen;
        if (copyLen < itemLen) {
            offset_ += copyLen;
        } else {
            offset_ = 0;
            buffer_.pop_front();
        }
    }while(nRemain != 0);
#ifdef OUT_PUT_TO_FILE
    if (poped.is_open()) {
        poped.write(pData, nDataLen);
    }
#endif
    return nDataLen;
}


int StreamCache::TakeOne(std::vector<char> &_item, int nMilliSecTimeout) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (buffer_.size() == 0) {
        if (nMilliSecTimeout > 0) {
            auto status = condition_.wait_for(lock, static_cast<std::chrono::milliseconds>(nMilliSecTimeout));
            if (status == std::cv_status::timeout) {
                return TIMEOUT;
            }
        } else {
            condition_.wait(lock);
        }
    }

    if (buffer_.size() == 0) {
        return UNKNOWN;
    }
    auto item = buffer_.front();
    buffer_.pop_front();

    _item.swap(item);
    return OK;
}
