#include "iceplayer.h"

#include <QFile>
#include <QDir>
#include <QtQuick/QQuickWindow>

#ifdef TUTK_ENABLE
#include "tutkclient.h"
#endif

#define THIS_FILE "iceplayer.cpp"


IcePlayer::IcePlayer()
  : m_t(0)
  , m_vRenderer(nullptr)
{
    connect(this, &QQuickItem::windowChanged, this, &IcePlayer::handleWindowChanged);
    m_vRenderer = nullptr;
    loginfo("pwd:{}", QDir::currentPath().toStdString());

    auto timerHandle = [this]() {
        while(!quit_) {
            os_sleep_ms(1000);
            updateStreamInfo();
        }
    };
    //updateStreamInfoTimerThread_ = std::thread(timerHandle);

    auto arender = [this]() {
        while(!quit_) {

            std::deque<std::shared_ptr<MediaFrame>>::size_type asize =0;
            std::shared_ptr<MediaFrame> aframe;
            int64_t aPts = -1;

            std::unique_lock<std::mutex> lock(audioMutex_);
            asize = Abuffer_.size();
            if (canRender_ == false || asize == 0) {
                audioCondition_.wait(lock);
                if (quit_) {
                    continue;
                }

                asize = Abuffer_.size();
                if (asize == 0){
                    continue;
                }
            }

            aframe = Abuffer_.front();
            Abuffer_.pop_front();
            aPts = aframe->pts;

            if (aframe.get() == nullptr) {
                continue;
            }

            if (!m_aRenderer.IsInited()){
                m_aRenderer.Init(aframe);
            }

            int64_t now = os_getmonotonictime_ms();
            int64_t diffRealTime = now - audioSysTimeBase_;
            int64_t diffFrameTime = aPts - firstAudioFramePts_;
            //qDebug() << "audio frame pts"<<aPts;
            if (diffRealTime - diffFrameTime > 500) {
                qDebug() << "audio frame late:"<<diffRealTime - diffFrameTime;
            }
            if (diffFrameTime - diffRealTime > 1) {
                os_sleep_ms(diffFrameTime - diffRealTime - 1);
            }

            lastRenderAudioPts = aPts;
            AVFrame * f = aframe->AvFrame();
            m_aRenderer.PushData(f->data[0], f->linesize[0]);
            emit audioReady();
        }
    };
    audioRenderThread_ = std::thread(arender);


    auto vrender = [this]() {
        while(!quit_) {
            std::deque<std::shared_ptr<MediaFrame>>::size_type  vsize = 0;
            std::shared_ptr<MediaFrame> vframe;
            int64_t vPts = -1;

            std::unique_lock<std::mutex> lock(videoMutex_);
            vsize = Vbuffer_.size();
            if (canRender_ == false ||vsize == 0) {
                videoCondition_.wait(lock);
                if (quit_) {
                    continue;
                }

                vsize = Vbuffer_.size();
                if (vsize == 0){
                    continue;
                }
            }

            vframe = Vbuffer_.front();
            Vbuffer_.pop_front();
            vPts = vframe->pts;

            if (vframe.get() == nullptr) {
                continue;
            }

            int64_t now = os_getmonotonictime_ms();
            int64_t diffRealTime = now - videoSysTimeBase_;
            int64_t diffFrameTime = vPts - firstVideoFramePts_;
            //qDebug() << "video frame pts"<<vPts;
            if (diffRealTime - diffFrameTime > 500) {
                qDebug() << "video frame late:"<<diffRealTime - diffFrameTime;
            }
            if (diffFrameTime - diffRealTime > 1) {
                os_sleep_ms(diffFrameTime - diffRealTime - 1);
            }
            lastRenderVideoPts = vPts;
            m_vRenderer->SetFrame(vframe);
            emit pictureReady();
        }
    };
    videoRenderThread_ = std::thread(vrender);

}

IcePlayer::~IcePlayer()
{
    Exit();
}

void IcePlayer::updateStreamInfo()
{

    if (pMediaSource.get())
        emit streamInfoUpdate(pMediaSource->GetStreamInfo().c_str());
}

void IcePlayer::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &IcePlayer::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &IcePlayer::cleanup, Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        //win->setClearBeforeRendering(false);
    }
}

void IcePlayer::cleanup()
{
    if (m_vRenderer) {
        delete m_vRenderer;
        m_vRenderer = nullptr;
    }
}

void IcePlayer::repaint()
{
    if (m_vRenderer) {
        if (window()) {
            loginfo("render one frame");
            window()->update();
        }
    }
}


void IcePlayer::sync()
{
    if (!m_vRenderer) {
        m_vRenderer = new QGLRenderer();
        connect(window(), &QQuickWindow::afterRendering, m_vRenderer, &QGLRenderer::paint, Qt::DirectConnection);
        connect(this, &IcePlayer::pictureReady, this, &IcePlayer::repaint, Qt::QueuedConnection);
    }
    QSize wsize = window()->size();
    logdebug("window size: width:{} height:{}", wsize.width(), wsize.height());

    QObject * obj = window()->findChild<QObject *>("player");
    if(obj != nullptr){

        QQuickItem * item = dynamic_cast<QQuickItem *>(obj);
        if (item != nullptr){
            QRectF rect = item->boundingRect();
            //qDebug()<<"      --->:boundingRect:"<<item->rect();
            logtrace("playerRect's boundingRect:({} {} {} {})",
                     rect.left(), rect.top(), rect.right(), rect.bottom());
            QRectF s = item->mapRectToScene(item->boundingRect());
            //qDebug()<<"      --->:map:"<< s;
            logtrace("playerRect's mapRectToScene:({} {} {} {})",
                     s.left(), s.top(), s.right(), s.bottom());

            QRect wrect = window()->geometry();
            //qDebug()<<"      --->:window rect:"<< wrect;
            logtrace("window geometry:({} {} {} {})",
                wrect.left(), wrect.top(), wrect.right(), wrect.bottom());

            QRectF c;
            c.setLeft(0.0);
            c.setBottom(0.0);
            c.setRight(window()->geometry().width());
            c.setBottom(window()->geometry().height());
            //qDebug()<<"      --->:window rect:"<< c;
            logtrace("window geometry:({} {} {} {})",
                c.left(), c.top(), c.right(), c.bottom());
            m_vRenderer->setDrawRect(c, s);
        }
    } else {
        logerror("not found player");
    }

    m_vRenderer->setViewportSize(wsize * window()->devicePixelRatio());
    m_vRenderer->setWindow(window());
}



void IcePlayer::play(int sourceType, QString audioOrAvFile, QString videoFile) {
    logdebug("iceplayer play {} {}", audioOrAvFile.toStdString(), videoFile.toStdString());
    qDebug()<<sourceType<<audioOrAvFile<<videoFile;
    setMediaSource(sourceType, audioOrAvFile, videoFile);
}

void IcePlayer::playAudio() {
    if(sourceType_ != 0) {
        return;
    }
    if (pMediaSource) {
        pMediaSource->PlayAudio();
    }
}

void IcePlayer::setMediaSource(int sourceType, QString audioOrAvFileOrUid, QString videoFileOrPwd) {

    if (sourceType == 2) {
        pMediaSource = std::make_shared<FileMediaSource>(IcePlayer::getFrameCallback, this,
                                                         audioOrAvFileOrUid.toLatin1().data(), videoFileOrPwd.toLatin1().data());
    } else if (sourceType == 1) {
        pMediaSource = std::make_shared<MediaSource>(IcePlayer::getFrameCallback, this,
                                                         audioOrAvFileOrUid.toStdString());
    } else if (sourceType == 0) {
#ifdef TUTK_ENABLE
        pMediaSource = std::make_shared<TutkClient>(IcePlayer::getFrameCallback, this,
                                                         audioOrAvFileOrUid.toStdString(), videoFileOrPwd.toStdString());
#endif
    }
    sourceType_ = sourceType;//stype.toInt();
    canRender_ = true;
}

void IcePlayer::stop() {
    canRender_ = false;
    if (pMediaSource.get()) {
        pMediaSource->Stop();
        pMediaSource.reset();
    }
    m_aRenderer.Uninit();
}

void IcePlayer::Exit() {
    audioMutex_.lock();
    quit_ = true;
    audioCondition_.notify_one();
    audioMutex_.unlock();

    videoMutex_.lock();
    videoCondition_.notify_one();
    videoMutex_.unlock();

    if (updateStreamInfoTimerThread_.joinable()) {
        updateStreamInfoTimerThread_.join();
    }
    if(audioRenderThread_.joinable()) {
        audioRenderThread_.join();
    }
    if (videoRenderThread_.joinable()) {
        videoRenderThread_.join();
    }
}

void IcePlayer::getFrameCallback(void * userData, std::shared_ptr<MediaFrame> & frame)
{
    //qDebug()<<"get one frame";
    IcePlayer * player = static_cast<IcePlayer *>(userData);
    std::unique_lock<std::mutex> lock(player->audioMutex_);
    if (player->canRender_ == false) {
        return;
    }

    player->push(std::move(frame));
}

static  void timeCheckAndAdjust(int64_t& firstFramePts, int64_t& sysTimeBase, int64_t& lastRenderPts,
                                int64_t currentFramePts, const char *av)
{
    if (firstFramePts == -1) {
        firstFramePts = currentFramePts;
    }
    if(sysTimeBase == -1) {
        sysTimeBase = os_getmonotonictime_ms();
        lastRenderPts = 0;
    } else {
        //如果回退了时间戳大于1.5s，重置sysTimeBase
        //input loop放文件时候需要
        int64_t diff = lastRenderPts - currentFramePts;
        if (diff < 0)
            diff = -diff;
        if (diff > 1500) {
            qDebug()<<av<<": last:"<<lastRenderPts<< " cur:"<<currentFramePts<< " base:"<<sysTimeBase;
            sysTimeBase = os_getmonotonictime_ms();
            firstFramePts = currentFramePts;
            lastRenderPts = 0;
        }
    }
}

void IcePlayer::push(std::shared_ptr<MediaFrame> && frame)
{

    if(frame->GetStreamType() == AVMEDIA_TYPE_AUDIO) {
        timeCheckAndAdjust(firstAudioFramePts_, audioSysTimeBase_, lastRenderAudioPts, frame->pts, "audio");
        logdebug("audio framepts:{}", frame->pts);
        Abuffer_.emplace_back(frame);
        audioCondition_.notify_one();
    } else {
        timeCheckAndAdjust(firstVideoFramePts_, videoSysTimeBase_, lastRenderVideoPts, frame->pts, "video");
        logdebug("video framepts:{}", frame->pts);
        Vbuffer_.emplace_back(frame);
        videoCondition_.notify_one();
    }
}

void IcePlayer::firstAudioPktTime(QString timestr) {
    qDebug()<<"afirst:"<<timestr;
    emit getFirstAudioPktTime(timestr);
}

void IcePlayer::firstVideoPktTime(QString timestr) {
    qDebug()<<"vfirst:"<<timestr;
    emit getFirstVideoPktTime(timestr);
}
