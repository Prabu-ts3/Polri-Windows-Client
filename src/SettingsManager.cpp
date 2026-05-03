#include "SettingsManager.h"
#include <QMediaDevices>
#include <QAudioDevice>

static SettingsManager* s_instance = nullptr;

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent), m_settings("Polri", "WindowsClient")
{
    s_instance = this;

    // Monitor hardware changes
    QMediaDevices *mediaDevices = new QMediaDevices(this);
    connect(mediaDevices, &QMediaDevices::audioInputsChanged, this, &SettingsManager::availableDevicesChanged);
    connect(mediaDevices, &QMediaDevices::audioOutputsChanged, this, &SettingsManager::availableDevicesChanged);
}

SettingsManager* SettingsManager::instance()
{
    return s_instance;
}

QString SettingsManager::getServerUrl() const
{
    return m_settings.value("serverUrl", "ws://polri.poc-id.my.id:5000").toString();
}

void SettingsManager::setServerUrl(const QString &url)
{
    if (getServerUrl() != url) {
        m_settings.setValue("serverUrl", url);
        emit serverUrlChanged();
    }
}

int SettingsManager::getOutputVolume() const
{
    return m_settings.value("outputVolume", 100).toInt();
}

void SettingsManager::setOutputVolume(int volume)
{
    if (getOutputVolume() != volume) {
        m_settings.setValue("outputVolume", volume);
        emit outputVolumeChanged();
    }
}

bool SettingsManager::isVoxEnabled() const
{
    return m_settings.value("voxEnabled", false).toBool();
}

void SettingsManager::setVoxEnabled(bool enabled)
{
    if (isVoxEnabled() != enabled) {
        m_settings.setValue("voxEnabled", enabled);
        emit voxEnabledChanged();
    }
}

bool SettingsManager::isPttToggle() const
{
    return m_settings.value("pttToggle", false).toBool();
}

void SettingsManager::setPttToggle(bool enabled)
{
    if (isPttToggle() != enabled) {
        m_settings.setValue("pttToggle", enabled);
        emit pttToggleChanged();
    }
}

bool SettingsManager::isSoundPush() const
{
    return m_settings.value("soundPush", true).toBool();
}

void SettingsManager::setSoundPush(bool enabled)
{
    if (isSoundPush() != enabled) {
        m_settings.setValue("soundPush", enabled);
        emit soundPushChanged();
    }
}

bool SettingsManager::isSoundRx() const
{
    return m_settings.value("soundRx", true).toBool();
}

void SettingsManager::setSoundRx(bool enabled)
{
    if (isSoundRx() != enabled) {
        m_settings.setValue("soundRx", enabled);
        emit soundRxChanged();
    }
}

bool SettingsManager::isGatewayMode() const
{
    return m_settings.value("gatewayMode", false).toBool();
}

void SettingsManager::setGatewayMode(bool enabled)
{
    if (isGatewayMode() != enabled) {
        m_settings.setValue("gatewayMode", enabled);
        emit gatewayModeChanged();
    }
}

bool SettingsManager::isDtmfMode() const
{
    return m_settings.value("dtmfMode", false).toBool();
}

void SettingsManager::setDtmfMode(bool enabled)
{
    if (isDtmfMode() != enabled) {
        m_settings.setValue("dtmfMode", enabled);
        emit dtmfModeChanged();
    }
}

int SettingsManager::getVoxThreshold() const
{
    return m_settings.value("voxThreshold", 2500).toInt();
}

void SettingsManager::setVoxThreshold(int threshold)
{
    if (getVoxThreshold() != threshold) {
        m_settings.setValue("voxThreshold", threshold);
        emit voxThresholdChanged();
    }
}

QString SettingsManager::getInputDeviceId() const
{
    return m_settings.value("inputDeviceId", "default").toString();
}

void SettingsManager::setInputDeviceId(const QString &id)
{
    if (getInputDeviceId() != id) {
        m_settings.setValue("inputDeviceId", id);
        emit inputDeviceIdChanged();
    }
}

QString SettingsManager::getOutputDeviceId() const
{
    return m_settings.value("outputDeviceId", "default").toString();
}

void SettingsManager::setOutputDeviceId(const QString &id)
{
    if (getOutputDeviceId() != id) {
        m_settings.setValue("outputDeviceId", id);
        emit outputDeviceIdChanged();
    }
}

QStringList SettingsManager::getAvailableInputs() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioInputs();
    for (const QAudioDevice &device : devices) {
        list << device.description();
    }
    return list;
}

QStringList SettingsManager::getAvailableOutputs() const
{
    QStringList list;
    const auto devices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : devices) {
        list << device.description();
    }
    return list;
}
