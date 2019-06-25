#ifndef TUTKCLIENT_H
#define TUTKCLIENT_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "IOTCAPIs.h"
#include "AVAPIs.h"
#include "P2PCam/AVFRAMEINFO.h"
#include "P2PCam/AVIOCTRLDEFs.h"

#include "mediasource.h"
#include <thread>
#include "streamcache.h"
#include "input.hpp"


class TutkClient : public MediaSource
{
public:
    TutkClient(OutputFrameCallback cb, void *userData, std::string uid, std::string pwd);
    ~TutkClient();

public:
    int Init();
    void Stop();
    MediaSource * CreateSource();
    void PlayAudio();

private:
    std::string uid_;
    std::string password_;
    std::thread connectCCRthread_;
    int SID_ = -1;
    int avIndex_ = -1;
    std::string avID_;

    bool isEnableAudio_ = false;
    std::thread audioThread_;

    std::thread videoThread_;

    int speakerChannel_ = -1;
    bool isEnableSpeaker_ = false;
    std::thread speakerThread_;

    int64_t usecTimeInfo_;

    StreamCache audioCache_;
    StreamCache videoCache_;
    std::vector<char> audioBufForTutk_;
    std::vector<char> videoBufForTutk_;

    bool isQuit_ = false;

    std::shared_ptr<Input> audiOInput_;
    std::shared_ptr<Input> videoInput_;
    ENUM_CODECID audioCodecId_ = MEDIA_CODEC_UNKNOWN;
    ENUM_CODECID videoCodecId_ = MEDIA_CODEC_UNKNOWN;
    bool canPushVideo = false;//测试，p2p送来的第一帧不是关键帧，这样解码可能有延迟

private:
    void connectCCRthread();
    int startIpcamStream();
    void speakerThread();
    void audioThread();
    void videoThread();

    int readFrame(FeedFrame *pFrame, StreamCache &cache);
    int readAudioFrame(FeedFrame *pFrame);
    int readVideoFrame(FeedFrame *pFrame);
};

#endif // TUTKCLIENT_H
