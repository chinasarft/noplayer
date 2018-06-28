#ifndef ICEPLAYER_H
#define ICEPLAYER_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtMultimedia>
#include <memory>
#include <input.hpp>
#include "linking.h"

class QGLRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
    friend class IcePlayer;
public:
    QGLRenderer();
    ~QGLRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }
    void setDrawRect(QRectF & s, QRectF &it);

    const int TextureYIndex = 0;
    const int TextureUIndex = 1;
    const int TextureVIndex = 2;
    void SetFrame(std::shared_ptr<MediaFrame> &frame);
    void ClearFrame();

public slots:
    void paint();

private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
    GLuint m_textures[3];
    GLfloat xleft;
    GLfloat xright;
    GLfloat ytop;
    GLfloat ybottom;

    std::mutex m_frameMutex;
    std::shared_ptr<MediaFrame> m_frame;
};

class AudioRender{
public:
    const static int ulawType = 1;
    const static int alawType = 2;
    AudioRender();
    void PushData(void *pcmData,int size);
    void PushG711Data(void *g711Data, int size, int lawType);
    void Init(QAudioFormat config);
    void Uninit();
    bool IsInited(){return m_inited;}
private:
    QAudioFormat m_audioConfig;
    std::shared_ptr<QAudioOutput> m_audioOutput;
    bool m_inited;
    QIODevice *m_device;
};

class IcePlayer : public QQuickItem
{
    Q_OBJECT

public:
    explicit IcePlayer();

signals:
    void tChanged();
    void pictureReady();

public slots:
    void sync();
    void cleanup();
    void Stop();
    void repaint();

    void call(QVariant sipAccount);
    void makeCall();
    void hangup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    static void getFrameCallback(void * userData, std::shared_ptr<MediaFrame> & frame);
    static int feedFrameCallbackAudio(void *, uint8_t *buf, int buf_size);
    static int feedFrameCallbackVideo(void *, uint8_t *buf, int buf_size);

private:
    qreal m_t;
    QGLRenderer *m_vRenderer;
    AudioRender m_aRenderer;

    std::shared_ptr<Input> m_stream1;
    std::shared_ptr<Input> m_stream2;

    std::shared_ptr<linking> iceSource_;
    std::shared_ptr<std::vector<uint8_t>> buffer_; //视频帧ffmpeg一次读取不完，所以需要记录下来下次读
    bool registerOk;
};

#endif // ICEPLAYER_H
