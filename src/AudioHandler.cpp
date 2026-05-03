#include "AudioHandler.h"
#include "SettingsManager.h"
#include <QDebug>
#include <QtMath>
#include <QTimer>
#include <QMutexLocker>

static AudioHandler* s_instance = nullptr;

AudioHandler::AudioHandler(QObject *parent) : QObject(parent)
{
    s_instance = this;
    setupAudio();

    int error;
    m_encoder = opus_encoder_create(m_sampleRate, m_channels, OPUS_APPLICATION_VOIP, &error);
    opus_encoder_ctl(m_encoder, OPUS_SET_BITRATE(24000));

    // Mixer Timer: Runs every 20ms (standard Opus frame size)
    m_mixTimer = new QTimer(this);
    connect(m_mixTimer, &QTimer::timeout, this, &AudioHandler::mixAndPlay);
    m_mixTimer->start(20);
}

AudioHandler::~AudioHandler()
{
    if (m_encoder) opus_encoder_destroy(m_encoder);
    for (auto decoder : m_decoders) {
        opus_decoder_destroy(decoder);
    }
}

AudioHandler* AudioHandler::instance()
{
    return s_instance;
}

void AudioHandler::setupAudio()
{
    m_format.setSampleRate(m_sampleRate);
    m_format.setChannelCount(m_channels);
    m_format.setSampleFormat(QAudioFormat::Int16);

    updateInputDevice();
    updateOutputDevice();

    connect(SettingsManager::instance(), &SettingsManager::inputDeviceIdChanged, this, &AudioHandler::updateInputDevice);
    connect(SettingsManager::instance(), &SettingsManager::outputDeviceIdChanged, this, &AudioHandler::updateOutputDevice);
}

void AudioHandler::updateInputDevice()
{
    if (m_audioSource) {
        m_audioSource->stop();
        m_audioSource->deleteLater();
    }

    QString selectedDesc = SettingsManager::instance()->inputDeviceId();
    QAudioDevice selectedDevice = QMediaDevices::defaultAudioInput();

    const auto devices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : devices) {
        if (device.description() == selectedDesc) {
            selectedDevice = device;
            break;
        }
    }

    m_audioSource = new QAudioSource(selectedDevice, m_format, this);
    qDebug() << "Input device updated to:" << selectedDevice.description();
}

void AudioHandler::updateOutputDevice()
{
    if (m_audioSink) {
        m_audioSink->stop();
        m_audioSink->deleteLater();
    }

    QString selectedDesc = SettingsManager::instance()->outputDeviceId();
    QAudioDevice selectedDevice = QMediaDevices::defaultAudioOutput();

    const auto devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : devices) {
        if (device.description() == selectedDesc) {
            selectedDevice = device;
            break;
        }
    }

    m_audioSink = new QAudioSink(selectedDevice, m_format, this);
    qDebug() << "Output device updated to:" << selectedDevice.description();
}

OpusDecoder* AudioHandler::getOrCreateDecoder(quint32 userId)
{
    if (!m_decoders.contains(userId)) {
        int error;
        OpusDecoder* decoder = opus_decoder_create(m_sampleRate, m_channels, &error);
        m_decoders.insert(userId, decoder);
        qDebug() << "Created new decoder for User ID:" << userId;
    }
    return m_decoders.value(userId);
}

void AudioHandler::startRecording()
{
    if (m_audioSource->state() == QAudio::ActiveState) return;

    m_inputDevice = m_audioSource->start();
    connect(m_inputDevice, &QIODevice::readyRead, this, &AudioHandler::readAudio);
}

void AudioHandler::stopRecording()
{
    m_audioSource->stop();
    if (m_inputDevice) {
        disconnect(m_inputDevice, &QIODevice::readyRead, this, &AudioHandler::readAudio);
        m_inputDevice = nullptr;
    }
    m_isVoxSpeaking = false;
}

void AudioHandler::readAudio()
{
    if (!m_inputDevice) return;

    QByteArray pcmData = m_inputDevice->readAll();
    if (pcmData.size() < m_frameSize * sizeof(int16_t)) return;

    const int16_t* samples = reinterpret_cast<const int16_t*>(pcmData.data());

    // VOX Logic
    if (SettingsManager::instance()->voxEnabled() || SettingsManager::instance()->gatewayMode()) {
        processVox(samples, m_frameSize);
    }

    // Encode and Emit only if user is actually "talking" (Manual or VOX)
    // We assume the actual transmission logic is controlled by WebSocketManager
    // but we emit data here.
    unsigned char opusData[1275];
    int bytesEncoded = opus_encode(m_encoder, samples, m_frameSize, opusData, sizeof(opusData));

    if (bytesEncoded > 0) {
        emit audioDataReady(QByteArray(reinterpret_cast<const char*>(opusData), bytesEncoded));
    }
}

void AudioHandler::processVox(const int16_t* pcm, int samples)
{
    int maxAmp = 0;
    for (int i = 0; i < samples; ++i) {
        int absVal = qAbs(pcm[i]);
        if (absVal > maxAmp) maxAmp = absVal;
    }

    int threshold = SettingsManager::instance()->voxThreshold();

    if (maxAmp > threshold) {
        m_voxSilenceFrames = 0;
        if (!m_isVoxSpeaking) {
            m_isVoxSpeaking = true;
            emit voxTriggered(true);
        }
    } else {
        m_voxSilenceFrames++;
        if (m_isVoxSpeaking && m_voxSilenceFrames > 50) { // ~1 second of silence
            m_isVoxSpeaking = false;
            emit voxTriggered(false);
        }
    }
}

// Implementasi Audio Filter mirip Android Native per User
void AudioHandler::applyAudioFilter(quint32 userId, int16_t* buffer, int length) {
    if (!m_filterStates.contains(userId)) {
        m_filterStates.insert(userId, FilterState());
    }
    FilterState &state = m_filterStates[userId];

    const float alphaLow = (2.0 * M_PI * 350.0 / (16000.0 + 2.0 * M_PI * 350.0));
    const float alphaMid = (2.0 * M_PI * 3000.0 / (16000.0 + 2.0 * M_PI * 3000.0));

    for (int i = 0; i < length; ++i) {
        float sample = buffer[i];
        state.lowState += alphaLow * (sample - state.lowState);
        float bass = state.lowState;
        state.midState += alphaMid * (sample - state.midState);
        float mid = state.midState - bass;
        float treble = sample - state.midState;

        float processed = (bass * 1.5f) + (mid * 1.2f) + (treble * 0.4f);
        buffer[i] = qBound<int16_t>(-32768, processed, 32767);
    }
}

void AudioHandler::playAudio(quint32 userId, const QByteArray &data)
{
    OpusDecoder* decoder = getOrCreateDecoder(userId);

    int16_t outPcm[5760];
    int samplesDecoded = opus_decode(decoder,
                                     reinterpret_cast<const unsigned char*>(data.data()),
                                     data.size(),
                                     outPcm,
                                     m_frameSize, 0);

    if (samplesDecoded > 0) {
        // Apply EQ Filter per user
        applyAudioFilter(userId, outPcm, samplesDecoded);

        // Put into queue for mixing
        QMutexLocker locker(&m_audioMutex);
        m_userAudioQueues[userId].append(QByteArray(reinterpret_cast<const char*>(outPcm), samplesDecoded * sizeof(int16_t)));

        // Safety: Limit queue size to avoid latency buildup
        if (m_userAudioQueues[userId].size() > 5) {
            m_userAudioQueues[userId].removeFirst();
        }
    }
}

void AudioHandler::mixAndPlay()
{
    QMutexLocker locker(&m_audioMutex);
    if (m_userAudioQueues.isEmpty()) return;

    if (m_audioSink->state() == QAudio::StoppedState || !m_outputDevice) {
        m_outputDevice = m_audioSink->start();
    }
    if (!m_outputDevice) return;

    int bytesPerFrame = m_frameSize * sizeof(int16_t);
    QVector<int32_t> mixedBuffer(m_frameSize, 0);
    bool hasData = false;

    QMutableMapIterator<quint32, QList<QByteArray>> i(m_userAudioQueues);
    while (i.hasNext()) {
        i.next();
        QList<QByteArray> &queue = i.value();
        if (!queue.isEmpty()) {
            QByteArray pcmData = queue.takeFirst();
            const int16_t* samples = reinterpret_cast<const int16_t*>(pcmData.data());
            int count = qMin(static_cast<int>(pcmData.size() / sizeof(int16_t)), m_frameSize);

            for (int j = 0; j < count; ++j) {
                mixedBuffer[j] += samples[j];
            }
            hasData = true;
        }

        // Clean up empty queues
        if (queue.isEmpty() && !m_decoders.contains(i.key())) {
            // keep it for a while but could be removed
        }
    }

    if (hasData) {
        QByteArray finalBuffer;
        finalBuffer.resize(bytesPerFrame);
        int16_t* outSamples = reinterpret_cast<int16_t*>(finalBuffer.data());

        float vol = SettingsManager::instance()->outputVolume() / 100.0f;

        for (int j = 0; j < m_frameSize; ++j) {
            // Clipping protection and volume application
            int32_t sample = static_cast<int32_t>(mixedBuffer[j] * vol);
            outSamples[j] = qBound<int16_t>(-32768, sample, 32767);
        }

        m_outputDevice->write(finalBuffer);
    }
}
