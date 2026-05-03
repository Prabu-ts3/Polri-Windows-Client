#include "VideoRenderer.h"
#include <QDebug>
#include <QVideoFrameFormat>
#include <QtConcurrent>

static VideoRenderer* s_instance = nullptr;

VideoRenderer::VideoRenderer(QObject *parent) : QObject(parent)
{
    s_instance = this;
}

VideoRenderer* VideoRenderer::instance()
{
    return s_instance;
}

void VideoRenderer::setVideoSink(QVideoSink* sink)
{
    if (m_videoSink != sink) {
        m_videoSink = sink;
        emit videoSinkChanged();
    }
}

void VideoRenderer::processFrame(const QByteArray &data)
{
    if (!m_videoSink) return;

    // Use QtConcurrent to decode the JPEG frame in a background thread
    // This prevents the WebSocket/Main thread from blocking during image decoding
    QtConcurrent::run([this, data]() {
        QImage image;
        if (image.loadFromData(data)) {
            // Convert QImage to QVideoFrame (expensive part done in thread)
            QVideoFrame frame(image);
            if (frame.isValid()) {
                // Update the sink on the main/UI thread
                QMetaObject::invokeMethod(this, [this, frame]() {
                    if (m_videoSink) {
                        m_videoSink->setVideoFrame(frame);
                    }
                }, Qt::QueuedConnection);
            }
        } else {
            qWarning() << "Failed to load video frame from binary data";
        }
    });
}
