#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "WebSocketManager.h"
#include "SettingsManager.h"
#include "VideoRenderer.h"
#include "LocationManager.h"
#include "SoundManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    WebSocketManager wsManager;
    SettingsManager settingsManager;
    VideoRenderer videoRenderer;
    LocationManager locationManager;
    SoundManager soundManager;

    QQmlApplicationEngine engine;

    // Memberikan akses ke QML
    engine.rootContext()->setContextProperty("wsManager", &wsManager);
    engine.rootContext()->setContextProperty("settingsManager", &settingsManager);
    engine.rootContext()->setContextProperty("videoRenderer", &videoRenderer);
    engine.rootContext()->setContextProperty("locationManager", &locationManager);

    // Mengatur modul QML
    const QUrl url(QStringLiteral("qrc:/PolriApp/res/qml/main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
