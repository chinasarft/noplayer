#ifndef STATISTICS_H
#define STATISTICS_H
#include <vector>
#include <thread>
#include <ThreadCleaner.h>
#include <mutex>

class Statistics : public StopClass
{
public:
    Statistics(int videoInterval = 10);
    void StatVideo(int size, bool isIDR);
    void StatAudio(int size);
    int GetVideoAvgRate() {return avgVideoRate_;}
    int GetAudioAvgRate() {return avgAudioRate_;}
    int GetVideoFps() {return videoFps_;}
    int GetAudioFps() {return audioFps_;}
    void Start();
    void Stop();
private:
    bool quit_ = false;
    int interval_ = 0;

    int64_t totalVideoByte_ = 0;
    int64_t curVideoStatByte_ = 0;
    int avgVideoRate_ = 0;
    int totalVideoFrameCount_ = 0;
    int videoFrameCount_ = 0;
    int videoFps_;
    std::vector<int> perSecond_;

    int64_t totalAudioByte_ = 0;
    int64_t curAudioStatByte_ = 0;
    int avgAudioRate_ = 0;
    int totalAudioFrameCount_ = 0;
    int audioFrameCount_ = 0;
    int audioFps_;

    std::thread statThread;
    std::mutex mutex_;
};

#endif // STATISTICS_H
