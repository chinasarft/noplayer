#include "mediasource.h"

MediaSource::MediaSource(OutputFrameCallback cb, void *userData, std::string fileName):
        outputFrameCallback(cb),
        pUserData(userData),
        inputFileName_(fileName)
{
    if (!isInited_) {
        if (Init() == 0)
            isInited_ = true;
    }
}

std::string MediaSource::GetStreamInfo()
{
    if (input_.get()) {
        return input_->GetMediaStatInfoStr();
    }
    return "no stream info";
}

int MediaSource::Init()
{
    if(!inputFileName_.empty()) {
        InputParam param1;
        param1.name_ = "media";
        param1.url_ = inputFileName_;
        param1.userData_ = pUserData;
        param1.getFrameCb_ = outputFrameCallback;

        input_ = std::make_shared<Input>(param1);
        input_->Start();
        return 0;
    }

    return -1;
}

void MediaSource::PlayAudio() {
    return;
}

void MediaSource::Stop()
{
    if (input_) {
        input_->Stop();
    }
    isInited_ = false;
    return;
}

int MediaSource::feedAudioDataToFFmpegCallback(void *opaque, uint8_t *buf, int buf_size)
{
    MediaSource *pSrc = static_cast<MediaSource *>(opaque);
    return pSrc->readAudioData(buf, buf_size);
}

int MediaSource::feedVideoDataToFFmpegCallback(void *opaque, uint8_t *buf, int buf_size)
{
    MediaSource *pSrc = static_cast<MediaSource *>(opaque);
    return pSrc->readVideoData(buf, buf_size);
}

int MediaSource::readAudioData(uint8_t *buf, int buf_size)
{
    (void)buf;
    (void)buf_size;
    return -1;
}

int MediaSource::readVideoData(uint8_t *buf, int buf_size)
{
    (void)buf;
    (void)buf_size;
    return -1;
}


int MediaSource::feedAudioDataToFFmpegDecoderCallback(void *opaque, FeedFrame *pFrame)
{
    MediaSource *pSrc = static_cast<MediaSource *>(opaque);
    return pSrc->readAudioFrame(pFrame);
}

int MediaSource::feedVideoDataToFFmpegDecoderCallback(void *opaque, FeedFrame *pFrame)
{
    MediaSource *pSrc = static_cast<MediaSource *>(opaque);
    //fprintf(stderr, "ready to read video frame:%x\n", pSrc->readAudioFrame);
    return pSrc->readVideoFrame(pFrame);
}


int MediaSource::readAudioFrame(FeedFrame *pFrame)
{
    (void)pFrame;
    return -1;
}

int MediaSource::readVideoFrame(FeedFrame *pFrame)
{
    (void)pFrame;
    return -1;
}

