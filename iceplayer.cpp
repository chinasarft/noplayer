#include "iceplayer.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QFile>
#include "g711.h"
#define THIS_FILE "iceplayer.cpp"

#include <QFile>

QFile audioFile("/Users/liuye/Documents/qml/iceplayer/a.mulaw");
static int audioFileReadSize = 0;
int audioFeed(void *opaque, uint8_t *buf, int buf_size)
{
    if(!audioFile.isOpen()) {
        audioFile.open(QIODevice::ReadOnly);
    }
    if(audioFile.isOpen()) {
        int n = audioFile.read((char *)buf, buf_size);
        if (n == 0)
            return 0;
        audioFileReadSize+=n;
        return n;
    }
    return -1;
}

QFile videoFile("/Users/liuye/Documents/qml/iceplayer/v.h264");
static int videoFileReadSize = 0;
int videoFeed(void *opaque, uint8_t *buf, int buf_size)
{
    if(!videoFile.isOpen()) {
        videoFile.open(QIODevice::ReadOnly);
    }
    if(videoFile.isOpen()) {
        int n = videoFile.read((char *)buf, buf_size);
        if (n == 0)
            return 0;
        videoFileReadSize+=n;
        return n;
    }
    return -1;
}


QGLRenderer::~QGLRenderer()
{
    delete m_program;
}

QGLRenderer::QGLRenderer() : m_program(0) {
    m_textures[0] = 0;
    m_textures[1] = 0;
    m_textures[2] = 0;
    xleft = -1.0f;
    xright = 1.0f;
    ytop = 1.0f;
    ybottom = -1.0f;
    if (!m_program) {
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "attribute highp vec3 vertexIn;\n"
                                                    "attribute highp vec2 textureIn;\n"
                                                    "varying vec2 textureOut;\n"
                                                    "void main(void)\n"
                                                    "{\n"
                                                    "	gl_Position = vec4(vertexIn, 1.0);\n"
                                                    "	textureOut = textureIn;\n"
                                                    "}\n");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    "varying vec2 textureOut;\n"
                                                    "uniform sampler2D tex0;\n"
                                                    "uniform sampler2D tex1;\n"
                                                    "uniform sampler2D tex2;\n"
                                                    "void main(void)\n"
                                                    "{\n"
                                                    "	vec3 yuv;\n"
                                                    "	vec3 rgb;\n"
                                                    "	yuv.x = texture2D(tex0, textureOut).r;\n"
                                                    "	yuv.y = texture2D(tex1, textureOut).r - 0.5;\n"
                                                    "	yuv.z = texture2D(tex2, textureOut).r - 0.5;\n"
                                                    "	rgb = mat3(1, 1, 1,\n"
                                                    "		0, -0.39465, 2.03211,\n"
                                                    "		1.13983, -0.58060, 0) * yuv;\n"
                                                    "	//gl_FragColor = vec4(0.0,0.0,1.0, 1);\n"
                                                    "	gl_FragColor = vec4(rgb, 1);\n"
                                                    "}\n");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();

        glGenTextures(3, m_textures);
        for (int i = 0; i < 3; i++) {
            glBindTexture(GL_TEXTURE_2D, m_textures[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
            // Set our texture parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    // Note that we set our container wrapping method to GL_CLAMP_TO_EDGE
            // Set texture filtering
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]); // Unbind texture when done, so we won't accidentily mess up our texture.
        }
    }
}

void QGLRenderer::SetFrame(std::shared_ptr<MediaFrame> &frame)
{
    std::lock_guard<std::mutex> lock(m_frameMutex);
    m_frame = frame;
}

void QGLRenderer::ClearFrame()
{
    std::lock_guard<std::mutex> lock(m_frameMutex);
    m_frame.reset();
}

void QGLRenderer::setDrawRect(QRectF & s, QRectF &it){
    qreal sw = (s.right() - s.left())/2;
    qreal sh = (s.bottom() - s.top())/2;

    xleft = (it.left() - s.left() - sw) / sw;
    xright = (it.right() - s.left() - sw) / sw;
    ytop = (s.bottom() - it.top() - sh) / sh;
    ybottom = (s.bottom() - it.bottom() - sh) / sh;

    logtrace("\n{} {}\n{} {}\n{} {}\n {} {}\{} {}\n{} {}",
            xleft, ybottom ,
            xright ,ytop,
            xleft , ytop,
            xleft , ybottom,
            xright ,ybottom,
            xright ,ytop);
}


void QGLRenderer::paint()
{
    float values[] = {
        //left top triangle
        xleft, ybottom, 0.0f,     0.0f, 1.0f,
        xright, ytop, 0.0f,       1.0f, 0.0f,
        xleft, ytop, 0.0f,      0.0f, 0.0f,
        //right down triangle
        xleft, ybottom, 0.0f,     0.0f, 1.0f,
        xright, ybottom, 0.0f,      1.0f, 1.0f,
        xright, ytop, 0.0f,       1.0f, 0.0f,
    };

    std::lock_guard<std::mutex> lock(m_frameMutex);
    AVFrame * f = nullptr;
    if (m_frame.get() != nullptr)
        f = m_frame->AvFrame();

    if (f != nullptr) {
        m_program->bind();
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);

        m_program->setAttributeArray(0, GL_FLOAT, values, 3, 5 * sizeof(GLfloat));
        m_program->setAttributeArray(1, GL_FLOAT, &values[3], 2, 5 * sizeof(GLfloat));

        for (int i = 0, j = 1; i < 3; i++, j = 2) {
            char name[5] = {0};
            sprintf(name, "tex%d", i);
            int location = m_program->uniformLocation(name);;
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i]);

            glPixelStorei(GL_UNPACK_ROW_LENGTH,
                          f->linesize[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                         f->width/j,
                         f->height/j, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, f->data[i]);
            glUniform1i(location, i);
        }

        glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

        //glDisable(GL_DEPTH_TEST);
        //glClearColor(0, 0, 0, 1);
        //glClear(GL_COLOR_BUFFER_BIT);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);


        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_program->disableAttributeArray(0);
        m_program->disableAttributeArray(1);
        m_program->release();

        // Not strictly needed for this example, but generally useful for when
        // mixing with raw OpenGL.
        m_window->resetOpenGLState();
        //glSwapAPPLE();
    }
}

AudioRender::AudioRender(){

}

void AudioRender::Init(QAudioFormat config){
    m_audioConfig = config;
    m_inited = false;
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(m_audioConfig)) {
        logerror("default format not supported try to use nearest");
        m_audioConfig = info.nearestFormat(m_audioConfig);
        return;
    }
    m_inited = true;
    m_audioOutput = std::make_shared<QAudioOutput>(m_audioConfig);
    m_device = m_audioOutput->start();
    m_audioOutput->setVolume(0.8);
    loginfo("init volume:{}", m_audioOutput->volume());
}

void AudioRender::Uninit(){
    if (m_device) {
        m_device->destroyed();
        m_device = nullptr;
    }

    if (m_audioOutput) {
        m_audioOutput->stop();
        m_audioOutput = nullptr;
        m_inited = false;
    }
}

void AudioRender::PushData(void *pcmData,int size){
    if(m_inited == false)
        return;

    m_device->write((const char *)pcmData, size);
}

void AudioRender::PushG711Data(void *g711Data, int size, int lawType){
    if(m_inited == false)
        return;
    short pcm[1280];
    unsigned char *src = (unsigned char*)g711Data;


    if (lawType == alawType) {
        for (int i = 0; i < size; i++) {
            pcm[i] = alaw2linear(src[i]);
        }
        PushData(pcm, size * 2);
    } else if (lawType == ulawType) {
        for (int i = 0; i < size; i++) {
            pcm[i] = ulaw2linear(src[i]);
        }
        PushData(pcm, size * 2);
    }
}

#include <QDir>
IcePlayer::IcePlayer()
  : m_t(0)
  , m_vRenderer(0)
  , registerOk(false)
{
    connect(this, &QQuickItem::windowChanged, this, &IcePlayer::handleWindowChanged);
    m_vRenderer = nullptr;
    loginfo("pwd:{}", QDir::currentPath().toStdString());
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
        m_vRenderer = 0;
    }
}

void IcePlayer::repaint()
{
    if (m_vRenderer) {
        if (window())
            window()->update();
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
#ifdef MORE_DETAILS
        QVariant h = obj->property("height");
        QVariant w = obj->property("width");
        logtrace("name:height type:{} value:{}", h.type().toString().toStdString(), h.toDouble());
        logtrace("name:width: type:{} value:{}", w.type().toString().toStdString(), w.toDouble());
        QVariant n = obj->property("objectName");
        logtrace("name:objectName type:{} value:{}", n.type().toString().toStdString(),
                 n.toString().toStdString());
        obj->dumpObjectInfo();
#endif

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

void IcePlayer::setSourceType(QVariant stype){
    sourceType_ = stype.toInt();
}

void IcePlayer::Stop() {
    logger_flush();
    qDebug()<<"iceplayer stop";
    if (m_stream1.get() != nullptr){
        m_stream1->Stop();
        m_stream1.reset();
        qDebug()<<"stream1 stop";
    }
    if (m_stream2.get() != nullptr){
        m_stream2->Stop();
        m_stream2.reset();
        qDebug()<<"stream2 stop";
    }

}

void IcePlayer::getFrameCallback(void * userData, std::shared_ptr<MediaFrame> & frame) {
    IcePlayer * player = (IcePlayer *)(userData);

    AVFrame * f = frame->AvFrame();
    if(frame->GetStreamType() == STREAM_AUDIO) {
        logdebug("audio framepts:{}", frame->pts);
        if (!player->m_aRenderer.IsInited()) {
            bool configOk = true;
            QAudioFormat config;
            config.setSampleRate(f->sample_rate);
            config.setChannelCount(f->channels);
            config.setCodec("audio/pcm");
            config.setByteOrder(QAudioFormat::LittleEndian);

            switch(f->format) {
            case AV_SAMPLE_FMT_U8:
                qDebug()<<"AV_SAMPLE_FMT_U8";
                config.setSampleSize(8);
                config.setSampleType(QAudioFormat::UnSignedInt);
                break;
            case AV_SAMPLE_FMT_S16:
                qDebug()<<"AV_SAMPLE_FMT_S16";
                config.setSampleSize(16);
                config.setSampleType(QAudioFormat::SignedInt);
                break;
            case AV_SAMPLE_FMT_S32:
                qDebug()<<"AV_SAMPLE_FMT_S32";
                config.setSampleSize(32);
                config.setSampleType(QAudioFormat::SignedInt);
                break;
            case AV_SAMPLE_FMT_FLT:
                qDebug()<<"AV_SAMPLE_FMT_FLT";
                config.setSampleSize(32);
                config.setSampleType(QAudioFormat::Float);
                break;

            case AV_SAMPLE_FMT_U8P:
            case AV_SAMPLE_FMT_S16P:
            case AV_SAMPLE_FMT_S32P:
            case AV_SAMPLE_FMT_FLTP:
            case AV_SAMPLE_FMT_DBL:
            case AV_SAMPLE_FMT_DBLP:
            case AV_SAMPLE_FMT_S64:
            case AV_SAMPLE_FMT_S64P:
                configOk = false;
                logerror("not soupport:{}", f->format);
                break;
            }
            if(configOk)
                player->m_aRenderer.Init(config);
        }

        if (player->m_aRenderer.IsInited()) {
            logdebug("playaudo: {}", f->linesize[0]);
            player->m_aRenderer.PushData(f->data[0], f->linesize[0]);
        }
    } else {
        logdebug("video framepts:{}", frame->pts);
        player->m_vRenderer->SetFrame(frame);
        emit player->pictureReady();
    }
}

void IcePlayer::call(QVariant sipAccount){
    QString strSipAcc = sipAccount.toString();
    qDebug()<<strSipAcc;
    if(sourceType_ == 0) {
        if (iceSource_.get() == nullptr) {
            iceSource_ = std::make_shared<linking>();
            iceSource_->SetCallee(strSipAcc.toStdString());
            qDebug()<<"first call"<<strSipAcc;
            connect(iceSource_.get(), SIGNAL(registerSuccess()), this, SLOT(makeCall()));
            connect(iceSource_.get(), SIGNAL(onFirstAudio(QString)), this, SLOT(firstAudioPktTime(QString)));
            connect(iceSource_.get(), SIGNAL(onFirstVideo(QString)), this, SLOT(firstVideoPktTime(QString)));
            return;
        }
        iceSource_->SetCallee(strSipAcc.toStdString());

        auto state = iceSource_->GetState();
        if (state == CALL_STATUS_IDLE || state == CALL_STATUS_REGISTER_FAIL) {
            logdebug("sip state wrong");
            return;
        }
    }
    makeCall();
}

void IcePlayer::firstAudioPktTime(QString timestr) {
    emit onFirstAudioPktTime(timestr);
}
void IcePlayer::firstVideoPktTime(QString timestr) {
    emit onFirstVideoPktTime(timestr);
}

void IcePlayer::makeCall(){
    qDebug()<<"qmlmakeCall";
    if(sourceType_ == 0) {
        iceSource_->call();
    }

    InputParam param1;
    InputParam param2;
    if(sourceType_ == 2 || sourceType_ == 0) {
        param1.userData_ = this;
        param1.name_ = "video";
        param1.feedCbOpaqueArg_ = this;
        param1.formatHint_ = "h264";
        param1.getFrameCb_ = IcePlayer::getFrameCallback;


        param2.userData_ = this;
        param2.name_ = "audio";
        param2.formatHint_ = "mulaw";
        param2.getFrameCb_ = IcePlayer::getFrameCallback;
        param2.feedCbOpaqueArg_ = this;
        param2.audioOpts.push_back("ar");
        param2.audioOpts.push_back("8000");


        param1.feedDataCb_ = videoFeed;
        param2.feedDataCb_ = audioFeed;
    }

    if(sourceType_ == 0) {
        param1.feedDataCb_ = feedFrameCallbackVideo;
        param2.feedDataCb_ = feedFrameCallbackAudio;
    }

    if (sourceType_ == 1) {
        param1.userData_ = this;
        param1.name_ = "test";
        param1.url_ = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
        param1.getFrameCb_ = IcePlayer::getFrameCallback;
    }

    loginfo("start stream1 and stream2");
    m_stream1 = std::make_shared<Input>(param1);
    m_stream1->Start();

    if(sourceType_ == 0 || sourceType_ == 2) {
        m_stream2 = std::make_shared<Input>(param2);
        m_stream2->Start();
    }
}

void IcePlayer::hangup(){
    qDebug()<<"hangup invoked";
    Stop();
    if(audioFile.isOpen()) {
        audioFile.close();
    }
    if(videoFile.isOpen()) {
        videoFile.close();
    }
#ifdef SIP_RTP_TEST
    if (iceSource_.get() != nullptr) {
        disconnect(iceSource_.get(), SIGNAL(registerSuccess()), this, SLOT(makeCall()));
        disconnect(iceSource_.get(), SIGNAL(onFirstAudio(QString)), this, SLOT(firstAudioPktTime(QString)));
        disconnect(iceSource_.get(), SIGNAL(onFirstVideo(QString)), this, SLOT(firstVideoPktTime(QString)));
        return;
        qDebug()<<"hangup call";
        iceSource_->hangup();
        iceSource_.reset();
    }
    Stop();

#endif
    m_aRenderer.Uninit();
    m_vRenderer->ClearFrame();
    window()->update();
}

//audio
int IcePlayer::feedFrameCallbackAudio(void *opaque, uint8_t *buf, int buf_size)
{
    IcePlayer *p = (IcePlayer*)opaque;
    std::shared_ptr<std::vector<uint8_t>> audioData;
    int times = 5;
    while(times > 0) {
        audioData = p->iceSource_->PopAudioData();
        if (audioData.get() == nullptr) {
            sleep(1);
            times--;
        } else {
            memcpy(buf, audioData->data(), audioData->size());
            return audioData->size();
        }
    }
    return 0;
}

//video
int IcePlayer::feedFrameCallbackVideo(void *opaque, uint8_t *buf, int buf_size)
{
    IcePlayer *p = (IcePlayer*)opaque;
    int times = 5;
    while(true) {
        if (times == 0)
            return 0;
        if (p->buffer_.get() != nullptr) {
            auto storeSize = p->buffer_->size();
            if (storeSize > 0) {
                if (buf_size >= storeSize) {
                    std::copy(p->buffer_->begin(), p->buffer_->end(), buf);
                    p->buffer_->resize(0);
                    return storeSize;
                } else {
                    std::copy(p->buffer_->begin(), p->buffer_->begin() + buf_size, buf);
                    std::copy(p->buffer_->begin() + buf_size, p->buffer_->end(), p->buffer_->begin());
                    p->buffer_->resize(storeSize - buf_size);
                    return buf_size;
                }
            }
        }

        std::shared_ptr<std::vector<uint8_t>> videoData;
        while(times > 0) {
            videoData = p->iceSource_->PopVideoData();
            if (videoData.get() == nullptr) {
                sleep(1);
                times--;
            } else {
                p->buffer_ = videoData;
                break;
            }
        }
    }
    return 0;
}
