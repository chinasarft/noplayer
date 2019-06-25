#ifndef FILEMEDIASOURCE_H
#define FILEMEDIASOURCE_H
#include "mediasource.h"
#include <fstream>
#include "input.hpp"

class FileMediaSource : public MediaSource
{
public:
    FileMediaSource(OutputFrameCallback cb, void *userData, const char *pAudioFile, const char *pVideoFile);
    int Init();
    void Stop();
    std::string GetStreamInfo();

private:
    std::string audioFile_;
    std::string videoFile_;
    std::fstream audioStream_;
    std::fstream videoStream_;

    std::shared_ptr<Input> audiOInput_;
    std::shared_ptr<Input> videoInput_;

private:
    int readAudioData(uint8_t *buf, int buf_size);
    int readVideoData(uint8_t *buf, int buf_size);

};

#endif // FILEMEDIASOURCE_H
