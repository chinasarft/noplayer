#ifndef ICEPLAYER_H
#define ICEPLAYER_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtMultimedia>
#include <memory>

class QGLRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    QGLRenderer();
    ~QGLRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }

    const int TextureYIndex = 0;
    const int TextureUIndex = 1;
    const int TextureVIndex = 2;

public slots:
    void paint();

private:
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
    GLuint m_textures[3];
};

class AudioRender{
public:
    const static int ulawType = 1;
    const static int alawType = 2;
    AudioRender();
    void PushData(void *pcmData,int size);
    void PushG711Data(void *g711Data, int size, int lawType);
private:
    QAudioFormat m_audioConfig;
    std::shared_ptr<QAudioOutput> m_audioOutput;
    bool m_canPlay;
    QIODevice *m_device;
};

class IcePlayer : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)

public:
    explicit IcePlayer();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    QGLRenderer *m_renderer;
};

#endif // ICEPLAYER_H
