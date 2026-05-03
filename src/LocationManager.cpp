#include "LocationManager.h"
#include "WebSocketManager.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

static LocationManager* s_instance = nullptr;

LocationManager::LocationManager(QObject *parent) : QObject(parent)
{
    s_instance = this;
    m_source = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_source) {
        connect(m_source, &QGeoPositionInfoSource::positionUpdated, this, &LocationManager::positionUpdated);
        m_source->setUpdateInterval(5000); // 5 seconds
        m_source->startUpdates();
    }
}

LocationManager* LocationManager::instance()
{
    return s_instance;
}

void LocationManager::positionUpdated(const QGeoPositionInfo &info)
{
    if (!info.isValid()) return;

    double lat = info.coordinate().latitude();
    double lon = info.coordinate().longitude();

    QJsonObject locData;
    locData["lat"] = lat;
    locData["lng"] = lon;

    // Send to server via WebSocketManager
    WebSocketManager::instance()->sendLocation(lat, lon);
}

void LocationManager::updateRemoteUsers(const QJsonArray &users)
{
    m_userLocations.clear();
    for (const QJsonValue &value : users) {
        QJsonObject obj = value.toObject();
        if (obj.contains("lat") && obj.contains("lng")) {
            QVariantMap marker;
            marker["userId"] = obj["id"].toString();
            marker["userName"] = obj["name"].toString();
            marker["latitude"] = obj["lat"].toDouble();
            marker["longitude"] = obj["lng"].toDouble();
            m_userLocations.append(marker);
        }
    }
    emit userLocationsChanged();
}
