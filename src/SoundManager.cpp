#include "SoundManager.h"
#include "SettingsManager.h"
#include <QUrl>
#include <QDebug>

static SoundManager* s_instance = nullptr;

SoundManager::SoundManager(QObject *parent) : QObject(parent)
{
    s_instance = this;

    // Volume levels identik dengan SoundManager.kt
    loadSound(PttOn, "qrc:/PolriApp/assets/sounds/on.wav", 0.2f);   // VOL_DTMF
    loadSound(PttOff, "qrc:/PolriApp/assets/sounds/off.wav", 0.2f); // VOL_DTMF
    loadSound(RxStart, "qrc:/PolriApp/assets/sounds/rx.wav", 0.5f); // VOL_RX_START
    loadSound(RxEnd, "qrc:/PolriApp/assets/sounds/end.wav", 0.8f);   // VOL_RX_STOP / VOL_END
    loadSound(Sos, "qrc:/PolriApp/assets/sounds/sos.wav", 1.0f);    // VOL_SOS
    loadSound(Push, "qrc:/PolriApp/assets/sounds/push.wav", 0.2f);  // VOL_PUSH
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

void SoundManager::play(SoundType type)
{
    if (m_sounds.contains(type)) {
        m_sounds[type]->play();
    }
}

void SoundManager::playStartTx()
{
    auto settings = SettingsManager::instance();
    if (!settings->soundPush()) return;
    if (settings->dtmfMode() || settings->gatewayMode()) return;

    play(Push);
}

void SoundManager::playStopTx()
{
    auto settings = SettingsManager::instance();
    if (!settings->soundPush()) return;
    if (settings->dtmfMode() || settings->gatewayMode()) return;

    play(RxEnd); // Di Android play(R.raw.end, VOL_END)
}

void SoundManager::playRxStart()
{
    auto settings = SettingsManager::instance();
    if (!settings->soundRx()) return;

    // Mode Gateway murni (tanpa DTMF) mematikan nada
    if (settings->gatewayMode() && !settings->dtmfMode()) return;

    if (settings->dtmfMode()) {
        play(PttOn); // on.wav
    } else {
        play(RxStart); // rx.wav
    }
}

void SoundManager::playRxStop()
{
    auto settings = SettingsManager::instance();
    if (!settings->soundRx()) return;

    // Mode Gateway murni (tanpa DTMF) mematikan nada
    if (settings->gatewayMode() && !settings->dtmfMode()) return;

    if (settings->dtmfMode()) {
        play(PttOff); // off.wav
    } else {
        play(RxEnd); // end.wav
    }
}

void SoundManager::startSosLoop()
{
    if (m_sounds.contains(Sos)) {
        m_sounds[Sos]->setLoopCount(QSoundEffect::Infinite);
        m_sounds[Sos]->play();
    }
}

void SoundManager::stopSosLoop()
{
    if (m_sounds.contains(Sos)) {
        m_sounds[Sos]->stop();
    }
}
