#include "SettingsManager.h"
#include <QMediaDevices>
#include <QAudioDevice>

static SettingsManager* s_instance = nullptr;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent), m_settings("Polri", "WindowsClient")
{
    s_instance = this;

    // Monitor hardware changes
    connect(QMediaDevices::instance(), &QMediaDevices::audioInputsChanged, this, &SettingsManager::availableDevicesChanged);
    connect(QMediaDevices::instance(), &QMediaDevices::audioOutputsChanged, this, &SettingsManager::availableDevicesChanged);
}

SettingsManager* SettingsManager::instance()
{
    return s_instance;
}

QString SettingsManager::serverUrl() const
{
    return m_settings.value("serverUrl", "ws://polri.poc-id.my.id:5000").toString();
}

void SettingsManager::setServerUrl(const QString &url)
{
    if (serverUrl() != url) {
        m_settings.setValue("serverUrl", url);
        emit serverUrlChanged();
    }
}

int SettingsManager::outputVolume() const
{
    return m_settings.value("outputVolume", 100).toInt();
}

void SettingsManager::setOutputVolume(int volume)
{
    if (outputVolume() != volume) {
        m_settings.setValue("outputVolume", volume);
        emit outputVolumeChanged();
    }
}

bool SettingsManager::voxEnabled() const
{
    return m_settings.value("voxEnabled", false).toBool();
}

void SettingsManager::setVoxEnabled(bool enabled)
{
    if (voxEnabled() != enabled) {
        m_settings.setValue("voxEnabled", enabled);
        emit voxEnabledChanged();
    }
}

bool SettingsManager::pttToggle() const
{
    return m_settings.value("pttToggle", false).toBool();
}

void SettingsManager::setPttToggle(bool enabled)
{
    if (pttToggle() != enabled) {
        m_settings.setValue("pttToggle", enabled);
        emit pttToggleChanged();
    }
}

bool SettingsManager::soundPush() const
{
    return m_settings.value("soundPush", true).toBool();
}

void SettingsManager::setSoundPush(bool enabled)
{
    if (soundPush() != enabled) {
        m_settings.setValue("soundPush", enabled);
        emit soundPushChanged();
    }
}

bool SettingsManager::soundRx() const
{
    return m_settings.value("soundRx", true).toBool();
}

void SettingsManager::setSoundRx(bool enabled)
{
    if (soundRx() != enabled) {
        m_settings.setValue("soundRx", enabled);
        emit soundRxChanged();
    }
}

bool SettingsManager::gatewayMode() const
{
    return m_settings.value("gatewayMode", false).toBool();
}

void SettingsManager::setGatewayMode(bool enabled)
{
    if (gatewayMode() != enabled) {
        m_settings.setValue("gatewayMode", enabled);
        emit gatewayModeChanged();
    }
}

bool SettingsManager::dtmfMode() const
{
    return m_settings.value("dtmfMode", false).toBool();
}

void SettingsManager::setDtmfMode(bool enabled)
{
    if (dtmfMode() != enabled) {
        m_settings.setValue("dtmfMode", enabled);
        emit dtmfModeChanged();
    }
}

int SettingsManager::voxThreshold() const
{
    return m_settings.value("voxThreshold", 2500).toInt();
}

void SettingsManager::setVoxThreshold(int threshold)
{
    if (voxThreshold() != threshold) {
        m_settings.setValue("voxThreshold", threshold);
        emit voxThresholdChanged();
    }
}

QString SettingsManager::inputDeviceId() const
{
    return m_settings.value("inputDeviceId", "default").toString();
}

void SettingsManager::setInputDeviceId(const QString &id)
{
    if (inputDeviceId() != id) {
        m_settings.setValue("inputDeviceId", id);
        emit inputDeviceIdChanged();
    }
}

QString SettingsManager::outputDeviceId() const
{
    return m_settings.value("outputDeviceId", "default").toString();
}

void SettingsManager::setOutputDeviceId(const QString &id)
{
    if (outputDeviceId() != id) {
        m_settings.setValue("outputDeviceId", id);
        emit outputDeviceIdChanged();
    }
}

QStringList SettingsManager::availableInputs() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : devices) {
        list << device.description();
    }
    return list;
}

QStringList SettingsManager::availableOutputs() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : devices) {
        list << device.description();
    }
    return list;
}
