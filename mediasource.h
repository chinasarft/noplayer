#ifndef MEDIASOURCE_H
#define MEDIASOURCE_H

#include <mutex>
#include <input.hpp>

typedef void (*OutputFrameCallback)(void * userData, std::shared_ptr<MediaFrame> & frame);

class MediaSourceData {
public:
    std::vector<char> data_;
};

class MediaSource
{
public:
    MediaSource(OutputFrameCallback cb, void *cbUserData, std::string fileName);
    virtual int Init();
    virtual void Stop();
    virtual std::string GetStreamInfo();
    int IsInited(){return isInited_;}
    virtual void PlayAudio();

    //判断文件名是否存在，存在直接使用文件输入，而不是avio通过回调送入数据
    std::string GetMediaFileName(){
        return inputFileName_;
    }

    //送入ffmpeg的数据来源，普通情况下是文件，但是如果是内存或者通过私有协议获取，这个时候MediaSource
    //子类就需要去包装这些私有协议获取数据源送入ffmpeg了
    static int feedAudioDataToFFmpegCallback(void *opaque, uint8_t *buf, int buf_size);
    static int feedVideoDataToFFmpegCallback(void *opaque, uint8_t *buf, int buf_size);

    //通常p2p 送过来的裸音视频，但是确带上了pts信息，这样可以直接生成AvPacket，所以要直接喂到decoder去
    static int feedAudioDataToFFmpegDecoderCallback(void *opaque, FeedFrame *pFrame);
    static int feedVideoDataToFFmpegDecoderCallback(void *opaque, FeedFrame *pFrame);

protected:
    OutputFrameCallback outputFrameCallback;
    void * pUserData;
    bool isInited_ = false;
    int nTimeout_ = 0;

private:
    std::string inputFileName_;
    std::shared_ptr<Input> input_;

private:
    virtual int readAudioFrame(FeedFrame *pFrame);
    virtual int readVideoFrame(FeedFrame *pFrame);
    virtual int readAudioData(uint8_t *buf, int buf_size);
    virtual int readVideoData(uint8_t *buf, int buf_size);

};

#endif // MEDIASOURCE_H
