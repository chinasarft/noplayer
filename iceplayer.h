#ifndef ICEPLAYER_H
#define ICEPLAYER_H

#include "render.h"
#include <QTimer>
#include "filemediasource.h"



class IcePlayer : public QQuickItem
{
    Q_OBJECT

public:
    explicit IcePlayer();
    ~IcePlayer();

signals:
    void tChanged();
    void pictureReady();
    void audioReady();
    void getFirstAudioPktTime(QString timestr);
    void getFirstVideoPktTime(QString timestr);
    void streamInfoUpdate(QString infoStr);

public slots:
    void sync();
    void repaint();

    void cleanup();
    void Exit();
    void stop();
    void setMediaSource(int sourceType, QString audioOrAvFile, QString videoFile);//TODO
    void play(int sourceType, QString audioOrAvFile, QString videoFile);
    void playAudio();

    void firstAudioPktTime(QString);
    void firstVideoPktTime(QString);

private slots:
    void handleWindowChanged(QQuickWindow *win);
    void updateStreamInfo();

private:
    static void getFrameCallback(void * userData, std::shared_ptr<MediaFrame> & frame);
    void push(std::shared_ptr<MediaFrame> && frame);

private:
    qreal m_t;
    QGLRenderer *m_vRenderer;
    AudioRender m_aRenderer;
    bool quit_ = false;

    int sourceType_ = 0; // 0 tutk, 1 file, 2 h264/pcmu file

    bool canRender_ = false;

    std::deque<std::shared_ptr<MediaFrame>> Abuffer_;
    std::mutex audioMutex_;
    std::condition_variable audioCondition_;
    std::thread audioRenderThread_;
    int64_t firstAudioFramePts_ = -1;
    int64_t audioSysTimeBase_ = -1;
    int64_t lastRenderAudioPts = -1;

    std::deque<std::shared_ptr<MediaFrame>> Vbuffer_;
    std::mutex videoMutex_;
    std::condition_variable videoCondition_;
    std::thread videoRenderThread_;
    int64_t firstVideoFramePts_ = -1;
    int64_t videoSysTimeBase_ = -1;
    int64_t lastRenderVideoPts = -1;

    std::thread updateStreamInfoTimerThread_;

    std::shared_ptr<MediaSource> pMediaSource;
};

#endif // ICEPLAYER_H
