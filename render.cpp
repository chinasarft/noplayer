#include "render.h"
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtQuick/qquickwindow.h>
#include "g711.h"
#include "logger.h"

#define THIS_FILE "render.cpp"

QGLRenderer::~QGLRenderer()
{
    delete m_program;
}

QGLRenderer::QGLRenderer() : m_program(nullptr) {
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
            glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));
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

void AudioRender::Init(std::shared_ptr<MediaFrame> & frame)
{
    AVFrame * f = frame->AvFrame();
    if (!m_inited) {
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
            Init(config);
        qDebug()<<"audio renderer configOk:"<<configOk << " rate:"<<f->sample_rate
               <<" channel:"<< f->channels << " initok:"<< IsInited();
    }
}

void AudioRender::Uninit(){
    if (m_inited) {
        if (m_device) {
            m_device->destroyed();
            m_device = nullptr;
        }

        if (m_audioOutput) {
            m_audioOutput->stop();
            m_audioOutput = nullptr;
        }
        m_inited = false;
    }
}

void AudioRender::PushData(void *pcmData,int size){
    if(m_inited == false)
        return;
    m_device->write(static_cast<const char *>(pcmData), size);
    //qint64 ret = m_device->write((const char *)pcmData, size);
    //qDebug()<<"wiret audio return:"<<ret;
}

void AudioRender::PushG711Data(void *g711Data, int size, int lawType){
    if(m_inited == false)
        return;
    short pcm[1280];
    unsigned char *src = static_cast<unsigned char*>(g711Data);


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
