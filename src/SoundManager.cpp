#include "SoundManager.h"
#include "SettingsManager.h"
#include <QUrl>
#include <QDebug>

static SoundManager* s_instance = nullptr;

SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
    s_instance = this;

    // Volume levels identik dengan SoundManager.kt
    loadSound(SoundPttOn, "qrc:/PolriApp/assets/sounds/on.wav", 0.2f);   // VOL_DTMF
    loadSound(SoundPttOff, "qrc:/PolriApp/assets/sounds/off.wav", 0.2f); // VOL_DTMF
    loadSound(SoundRxStart, "qrc:/PolriApp/assets/sounds/rx.wav", 0.5f); // VOL_RX_START
    loadSound(SoundRxEnd, "qrc:/PolriApp/assets/sounds/end.wav", 0.8f);   // VOL_RX_STOP / VOL_END
    loadSound(SoundSos, "qrc:/PolriApp/assets/sounds/sos.wav", 1.0f);    // VOL_SOS
    loadSound(SoundPush, "qrc:/PolriApp/assets/sounds/push.wav", 0.2f);  // VOL_PUSH
}

SoundManager* SoundManager::instance()
{
    return s_instance;
}

void SoundManager::loadSound(SoundType type, const QString &path, float volume)
{
    QSoundEffect *effect = new QSoundEffect(this);
    effect->setSource(QUrl(path));
    effect->setVolume(volume);
    m_sounds.insert(type, effect);
}

void SoundManager::play(SoundManager::SoundType type)
{
    if (m_sounds.contains(type)) {
        m_sounds[type]->play();
    }
}

void SoundManager::playStartTx()
{
    auto settings = SettingsManager::instance();
    if (!settings->isSoundPush()) return;
    if (settings->isDtmfMode() || settings->isGatewayMode()) return;

    play(SoundPush);
}

void SoundManager::playStopTx()
{
    auto settings = SettingsManager::instance();
    if (!settings->isSoundPush()) return;
    if (settings->isDtmfMode() || settings->isGatewayMode()) return;

    play(SoundRxEnd); // Di Android play(R.raw.end, VOL_END)
}

void SoundManager::playRxStart()
{
    auto settings = SettingsManager::instance();
    if (!settings->isSoundRx()) return;

    // Mode Gateway murni (tanpa DTMF) mematikan nada
    if (settings->isGatewayMode() && !settings->isDtmfMode()) return;

    if (settings->isDtmfMode()) {
        play(SoundPttOn); // on.wav
    } else {
        play(SoundRxStart); // rx.wav
    }
}

void SoundManager::playRxStop()
{
    auto settings = SettingsManager::instance();
    if (!settings->isSoundRx()) return;

    // Mode Gateway murni (tanpa DTMF) mematikan nada
    if (settings->isGatewayMode() && !settings->isDtmfMode()) return;

    if (settings->isDtmfMode()) {
        play(SoundPttOff); // off.wav
    } else {
        play(SoundRxEnd); // end.wav
    }
}

void SoundManager::startSosLoop()
{
    if (m_sounds.contains(SoundSos)) {
        m_sounds[SoundSos]->setLoopCount(QSoundEffect::Infinite);
        m_sounds[SoundSos]->play();
    }
}

void SoundManager::stopSosLoop()
{
    if (m_sounds.contains(SoundSos)) {
        m_sounds[SoundSos]->stop();
    }
}
