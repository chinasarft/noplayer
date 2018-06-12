#include "iceplayer.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QDebug>
#include <QFile>

#define VW 480
#define VH 288
typedef struct YuvData{
    char* pos[3];
    char data[VW*VH*2];
    int frameLen;
    int w;
    int h;
    YuvData(){
        pos[0] =  data;
        pos[1] = data + VW*VH;
        pos[2] = pos[1] + VW*VH/4;
        frameLen = VW * VH * 3 / 2;
        w = VW;
        h = VH;
    }
}YuvData;
YuvData yuvData;

QGLRenderer::~QGLRenderer()
{
    delete m_program;
}

QGLRenderer::QGLRenderer() : m_t(0), m_program(0) {
    m_textures[0] = 0;
    m_textures[1] = 0;
    m_textures[2] = 0;
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


void QGLRenderer::paint()
{
    m_program->bind();
    m_program->enableAttributeArray(0);
#if 1
    m_program->enableAttributeArray(1);
    float values[] = {
        //left top triangle
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,      0.0f, 0.0f,
        //right down triangle
        -1.0f, -1.0f, 0.0f,     0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,      1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f,
    #if 0
        //left down triangle
        1.0f, -1.0f, 0.0f,       1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f,       0.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,      0.0f, 1.0f,
        //right top triangle
        1.0f, -1.0f, 0.0f,     1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,      0.0f, 0.0f,
    #endif
    };

    //m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setAttributeArray(0, GL_FLOAT, values, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeArray(1, GL_FLOAT, &values[3], 2, 5 * sizeof(GLfloat));

    for (int i = 0, j = 1; i < 3; i++, j = 2) {
        char name[5] = {0};
        sprintf(name, "tex%d", i);
        int location = m_program->uniformLocation(name);;
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, yuvData.w/j,
                     yuvData.h/j, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, yuvData.pos[i]);
        glUniform1i(location, i);
    }

    int colorLocation = m_program->uniformLocation("color");
    QColor color(255, 0, 0, 100);
    m_program->setUniformValue(colorLocation, color);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);

#else
    float values[] = {
            -1, -1,
            1, -1,
            -1, 1,
            1, 1
    };
    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setUniformValue("t", (float) m_t);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

#endif
    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();
}

#include <QDir>
IcePlayer::IcePlayer()
  : m_t(0)
  , m_renderer(0)
{
    connect(this, &QQuickItem::windowChanged, this, &IcePlayer::handleWindowChanged);

    qDebug()<<QDir::currentPath();
    QFile file("/Users/liuye/Documents/qml/iceplayer/hks_480_288.yuv");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can't open the file!";
    } else {
        printf("frameLen:%d  base dataAddr:%p\n", yuvData.frameLen, &yuvData.data);
        printf("pos0=%p pos1=%p pos2=%p\n", yuvData.pos[0], yuvData.pos[1], yuvData.pos[2]);
        printf("pos2-pos1=%p, pos1-pos0=%p\n", yuvData.pos[2] - yuvData.pos[1], yuvData.pos[1] - yuvData.pos[0]);
        file.read(yuvData.data, yuvData.frameLen);
    }
}


void IcePlayer::setT(qreal t)
{
    if (t == m_t)
        return;
    m_t = t;
    emit tChanged();
    if (window())
        window()->update();
}

void IcePlayer::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &IcePlayer::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &IcePlayer::cleanup, Qt::DirectConnection);
        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
        win->setClearBeforeRendering(false);
    }
}

void IcePlayer::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void IcePlayer::sync()
{
    if (!m_renderer) {
        m_renderer = new QGLRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &QGLRenderer::paint, Qt::DirectConnection);
    }
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}
