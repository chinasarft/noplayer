#ifndef READER_H
#define READER_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtMultimedia>
#include <memory>
#include <input.hpp>

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
    void Init(std::shared_ptr<MediaFrame> & frame);
    void Uninit();
    bool IsInited(){return m_inited;}

    void SetFrame(std::shared_ptr<MediaFrame> &frame);
    void ClearFrame();
private:
    QAudioFormat m_audioConfig;
    std::shared_ptr<QAudioOutput> m_audioOutput;
    bool m_inited = false;
    QIODevice *m_device;

    std::mutex m_frameMutex;
    std::shared_ptr<MediaFrame> m_frame;
};

#endif // READER_H
