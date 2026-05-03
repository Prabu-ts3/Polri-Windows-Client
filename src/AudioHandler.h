#ifndef AUDIOHANDLER_H
#define AUDIOHANDLER_H

#include <QObject>
#include <QAudioSource>
#include <QAudioSink>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QIODevice>
#include <QByteArray>
#include <QMap>
#include <opus.h>

class AudioHandler : public QObject
{
    Q_OBJECT
public:
    explicit AudioHandler(QObject *parent = nullptr);
    ~AudioHandler();

    static AudioHandler* instance();

    void startRecording();
    void stopRecording();
    void playAudio(quint32 userId, const QByteArray &data);

private slots:
    void readAudio();
    void mixAndPlay();
    void updateInputDevice();
    void updateOutputDevice();

private:
    void setupAudio();
    OpusDecoder* getOrCreateDecoder(quint32 userId);
    void processVox(const int16_t* pcm, int samples);
    void applyAudioFilter(quint32 userId, int16_t* buffer, int length);

    QAudioSource *m_audioSource = nullptr;
    QAudioSink *m_audioSink = nullptr;
    QIODevice *m_inputDevice = nullptr;
    QIODevice *m_outputDevice = nullptr;
    QAudioFormat m_format;
    QTimer *m_mixTimer = nullptr;

    // Multi-user Buffers
    QMap<quint32, QList<QByteArray>> m_userAudioQueues;
    QMutex m_audioMutex;

    // Opus members
    OpusEncoder *m_encoder = nullptr;
    QMap<quint32, OpusDecoder*> m_decoders;

    struct FilterState {
        float lowState = 0;
        float midState = 0;
    };
    QMap<quint32, FilterState> m_filterStates;

    int m_sampleRate = 16000;
    int m_channels = 1;
    int m_frameSize = 320;

    // VOX state
    bool m_isVoxSpeaking = false;
    int m_voxSilenceFrames = 0;
    int m_voxTriggerFrames = 0;
};

#endif // AUDIOHANDLER_H
