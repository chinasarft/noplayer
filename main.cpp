#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "iceplayer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);


    qmlRegisterType<IcePlayer>("OpenGLUnderQML", 1, 0, "IcePlayer");

    logger_init_file_output("player.log");
    logger_set_level_debug();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();

}
