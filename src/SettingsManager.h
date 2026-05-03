#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int outputVolume READ outputVolume WRITE setOutputVolume NOTIFY outputVolumeChanged)
    Q_PROPERTY(bool voxEnabled READ voxEnabled WRITE setVoxEnabled NOTIFY voxEnabledChanged)
    Q_PROPERTY(bool pttToggle READ pttToggle WRITE setPttToggle NOTIFY pttToggleChanged)
    Q_PROPERTY(bool soundPush READ soundPush WRITE setSoundPush NOTIFY soundPushChanged)
    Q_PROPERTY(bool soundRx READ soundRx WRITE setSoundRx NOTIFY soundRxChanged)
    Q_PROPERTY(bool gatewayMode READ gatewayMode WRITE setGatewayMode NOTIFY gatewayModeChanged)
    Q_PROPERTY(bool dtmfMode READ dtmfMode WRITE setDtmfMode NOTIFY dtmfModeChanged)
    Q_PROPERTY(int voxThreshold READ voxThreshold WRITE setVoxThreshold NOTIFY voxThresholdChanged)
    Q_PROPERTY(QString inputDeviceId READ inputDeviceId WRITE setInputDeviceId NOTIFY inputDeviceIdChanged)
    Q_PROPERTY(QString outputDeviceId READ outputDeviceId WRITE setOutputDeviceId NOTIFY outputDeviceIdChanged)
    Q_PROPERTY(QStringList availableInputs READ availableInputs NOTIFY availableDevicesChanged)
    Q_PROPERTY(QStringList availableOutputs READ availableOutputs NOTIFY availableDevicesChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);
    static SettingsManager* instance();

    QString serverUrl() const;
    void setServerUrl(const QString &url);

    int outputVolume() const;
    void setOutputVolume(int volume);

    bool voxEnabled() const;
    void setVoxEnabled(bool enabled);

    bool pttToggle() const;
    void setPttToggle(bool enabled);

    bool soundPush() const;
    void setSoundPush(bool enabled);

    bool soundRx() const;
    void setSoundRx(bool enabled);

    bool gatewayMode() const;
    void setGatewayMode(bool enabled);

    bool dtmfMode() const;
    void setDtmfMode(bool enabled);

    int voxThreshold() const;
    void setVoxThreshold(int threshold);

    QString inputDeviceId() const;
    void setInputDeviceId(const QString &id);

    QString outputDeviceId() const;
    void setOutputDeviceId(const QString &id);

    QStringList availableInputs() const;
    QStringList availableOutputs() const;

signals:
    void serverUrlChanged();
    void outputVolumeChanged();
    void voxEnabledChanged();
    void pttToggleChanged();
    void soundPushChanged();
    void soundRxChanged();
    void gatewayModeChanged();
    void dtmfModeChanged();
    void voxThresholdChanged();
    void inputDeviceIdChanged();
    void outputDeviceIdChanged();
    void availableDevicesChanged();

private:
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H
