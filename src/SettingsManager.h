#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverUrl READ getServerUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int outputVolume READ getOutputVolume WRITE setOutputVolume NOTIFY outputVolumeChanged)
    Q_PROPERTY(bool voxEnabled READ isVoxEnabled WRITE setVoxEnabled NOTIFY voxEnabledChanged)
    Q_PROPERTY(bool pttToggle READ isPttToggle WRITE setPttToggle NOTIFY pttToggleChanged)
    Q_PROPERTY(bool soundPush READ isSoundPush WRITE setSoundPush NOTIFY soundPushChanged)
    Q_PROPERTY(bool soundRx READ isSoundRx WRITE setSoundRx NOTIFY soundRxChanged)
    Q_PROPERTY(bool gatewayMode READ isGatewayMode WRITE setGatewayMode NOTIFY gatewayModeChanged)
    Q_PROPERTY(bool dtmfMode READ isDtmfMode WRITE setDtmfMode NOTIFY dtmfModeChanged)
    Q_PROPERTY(int voxThreshold READ getVoxThreshold WRITE setVoxThreshold NOTIFY voxThresholdChanged)
    Q_PROPERTY(QString inputDeviceId READ getInputDeviceId WRITE setInputDeviceId NOTIFY inputDeviceIdChanged)
    Q_PROPERTY(QString outputDeviceId READ getOutputDeviceId WRITE setOutputDeviceId NOTIFY outputDeviceIdChanged)
    Q_PROPERTY(QStringList availableInputs READ getAvailableInputs NOTIFY availableDevicesChanged)
    Q_PROPERTY(QStringList availableOutputs READ getAvailableOutputs NOTIFY availableDevicesChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);
    static SettingsManager* instance();

    QString getServerUrl() const;
    void setServerUrl(const QString &url);

    int getOutputVolume() const;
    void setOutputVolume(int volume);

    bool isVoxEnabled() const;
    void setVoxEnabled(bool enabled);

    bool isPttToggle() const;
    void setPttToggle(bool enabled);

    bool isSoundPush() const;
    void setSoundPush(bool enabled);

    bool isSoundRx() const;
    void setSoundRx(bool enabled);

    bool isGatewayMode() const;
    void setGatewayMode(bool enabled);

    bool isDtmfMode() const;
    void setDtmfMode(bool enabled);

    int getVoxThreshold() const;
    void setVoxThreshold(int threshold);

    QString getInputDeviceId() const;
    void setInputDeviceId(const QString &id);

    QString getOutputDeviceId() const;
    void setOutputDeviceId(const QString &id);

    QStringList getAvailableInputs() const;
    QStringList getAvailableOutputs() const;

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
