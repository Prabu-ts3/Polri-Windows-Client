#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QSoundEffect>
#include <QMap>

class SoundManager : public QObject
{
    Q_OBJECT
public:
    explicit SoundManager(QObject *parent = nullptr);
    static SoundManager* instance();

    enum SoundType {
        SoundPttOn,
        SoundPttOff,
        SoundRxStart,
        SoundRxEnd,
        SoundSos,
        SoundPush
    };
    Q_ENUM(SoundType)

    Q_INVOKABLE void play(SoundManager::SoundType type);

    // Identik dengan Android SoundManager.kt
    void playStartTx();
    void playStopTx();
    void playRxStart();
    void playRxStop();
    void startSosLoop();
    void stopSosLoop();

private:
    QMap<SoundType, QSoundEffect*> m_sounds;
    void loadSound(SoundType type, const QString &path, float volume = 1.0f);
};

#endif // SOUNDMANAGER_H
