#include "filemediasource.h"

FileMediaSource::FileMediaSource(OutputFrameCallback cb,void *userData, const char *pAudioFile, const char *pVideoFile):
    MediaSource(cb, userData, ""),
    audioFile_(pAudioFile),
    videoFile_(pVideoFile)
{
    if (!isInited_) {
        if (Init() == 0)
            isInited_ = true;
    }
}

int FileMediaSource::Init()
{
    InputParam param1;
    InputParam param2;

    if (!audioFile_.empty()) {
        param1.name_ = "audio";
        param1.feedCbOpaqueArg_ = this;
        param1.feedDataCb_ = MediaSource::feedAudioDataToFFmpegCallback;
        param1.userData_ = pUserData;
        param1.getFrameCb_ = outputFrameCallback;
        param1.formatHint_ = "mulaw";
        param1.audioOpts.push_back("ar");
        param1.audioOpts.push_back("8000");

        audiOInput_ = std::make_shared<Input>(param1);
        audiOInput_->Start();
        if (videoFile_.empty())
            return 0;
    }

    if (!videoFile_.empty()) {
        param2.name_ = "audio";
        param2.feedCbOpaqueArg_ = this;
        param2.feedDataCb_ = MediaSource::feedVideoDataToFFmpegCallback;
        param2.formatHint_ = "h264";
        param2.userData_ = pUserData;
        param2.getFrameCb_ = outputFrameCallback;

        videoInput_ = std::make_shared<Input>(param2);
        videoInput_->Start();
        return 0;
    }

    return -1;
}

std::string FileMediaSource::GetStreamInfo()
{
    MediaStatInfo info;
    if (audiOInput_.get()) {
        info.Add(audiOInput_->GetMediaStatInfo());
    }

    if (videoInput_.get()) {
        info.Add(videoInput_->GetMediaStatInfo());
    }

    char infoStr[128] = {0};
    sprintf(infoStr, "vFps:%d vBr:%d kbps vCount:%d | aFps:%d aBr:%d aCount:%d",
            info.videoFps, info.videoBitrate * 8 / 1000, info.totalVideoFrameCount,
            info.audioFps, info.audioBitrate * 8 / 1000, info.totalAudioFrameCount);
    return infoStr;
}


void FileMediaSource::Stop(){
    if (videoInput_) {
        videoInput_->Stop();
        videoInput_.reset();
    }
    if (audiOInput_) {
        audiOInput_->Stop();
        audiOInput_.reset();
    }
    isInited_ = false;
}

int FileMediaSource::readAudioData(uint8_t *buf, int buf_size)
{

    if (!audioStream_.is_open()) {
        audioStream_.open(audioFile_,std::ios::in | std::ios::binary);
    }
    if (audioStream_.is_open()) {
        if (audioStream_.eof()) {
            audioStream_.close();
            audioStream_.open(audioFile_,std::ios::in | std::ios::binary);
        }
    }
    if (audioStream_.is_open()) {
        if(audioStream_.read(reinterpret_cast<char *>(buf), buf_size)) {
            return static_cast<int>(audioStream_.gcount());
        }
    }
    return -1;
}

int FileMediaSource::readVideoData(uint8_t *buf, int buf_size)
{
    if (!videoStream_.is_open()) {
        videoStream_.open(videoFile_,std::ios::in | std::ios::binary);
    }
    if (videoStream_.is_open()) {
        if (videoStream_.eof()) {
            videoStream_.close();
            videoStream_.open(videoFile_,std::ios::in | std::ios::binary);
        }
    }
    if (videoStream_.is_open()) {
        if(videoStream_.read(reinterpret_cast<char *>(buf), buf_size)) {
            return static_cast<int>(videoStream_.gcount());
        }
    }
    return -1;
}
