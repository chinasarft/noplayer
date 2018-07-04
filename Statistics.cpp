#include "Statistics.h"
#include <input.hpp>

Statistics::Statistics(int interval) :
    interval_(interval)
{
    perSecond_.resize(interval_);
    for (int i = 0; i < interval_; i++) {
        perSecond_[i] = 0;
    }
}

void Statistics::Start()
{
    auto w = [this]() {
        int vCount = 0; //每一次循环加1
        while(!quit_) {
            os_sleep_ms(1000);
            std::lock_guard<std::mutex> lock(mutex_);

            perSecond_[vCount % interval_] = curVideoStatByte_;
            vCount++;

            int base = vCount < interval_ ? vCount : interval_;
            int tmp = 0;
            for(int i = 0; i < interval_; i++) {
                tmp += perSecond_[i];
            }
            avgVideoRate_ = tmp / base;

            videoFps_ = videoFrameCount_;
            audioFps_ = audioFrameCount_;
            avgAudioRate_ = curAudioStatByte_;

            curVideoStatByte_ = 0;
            curAudioStatByte_ = 0;
            videoFrameCount_ = 0;
            audioFrameCount_ = 0;
        }
    };
    statThread = std::thread(w);
}

void Statistics::Stop()
{
    quit_ = true;
    if (statThread.joinable()) {
        statThread.join();
    }
}

void Statistics::StatVideo(int size, bool isIDR)
{
    std::lock_guard<std::mutex> lock(mutex_);
    totalVideoByte_ += size;
    videoFrameCount_++;
    curVideoStatByte_ += size;
}

void Statistics::StatAudio(int size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    totalAudioByte_ += size;
    audioFrameCount_++;
    curAudioStatByte_ += size;
}
