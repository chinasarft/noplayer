#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "iceplayer.h"
#define THIS_FILE "qmlmain.cpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    // OpenGLUnderQML is import name from qml(see main.qml)
    qmlRegisterType<IcePlayer>("OpenGLUnderQML", 1, 0, "IcePlayer");

    logger_init_file_output("/tmp/player.log");
    logger_set_level_debug();
    //logger_set_level_trace();

    QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    engine.load(QUrl::fromLocalFile("main.qml"));
    if (engine.rootObjects().isEmpty())
        return -1;

    int ret = app.exec();

    return ret;
}
