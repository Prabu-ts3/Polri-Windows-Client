#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariantList>

class LocationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList userLocations READ userLocations NOTIFY userLocationsChanged)

public:
    explicit LocationManager(QObject *parent = nullptr);
    static LocationManager* instance();

    QVariantList userLocations() const { return m_userLocations; }

    void updateRemoteUsers(const QJsonArray &users);

signals:
    void userLocationsChanged();

private slots:
    void positionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *m_source = nullptr;
    QVariantList m_userLocations;
};

#endif // LOCATIONMANAGER_H
