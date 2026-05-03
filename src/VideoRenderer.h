#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QImage>

class VideoRenderer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVideoSink* videoSink READ getVideoSink WRITE setVideoSink NOTIFY videoSinkChanged)

public:
    explicit VideoRenderer(QObject *parent = nullptr);
    static VideoRenderer* instance();

    QVideoSink* getVideoSink() const { return m_videoSink; }
    void setVideoSink(QVideoSink* sink);

    void processFrame(const QByteArray &data);

signals:
    void videoSinkChanged();

private:
    QVideoSink* m_videoSink = nullptr;
};

#endif // VIDEORENDERER_H
