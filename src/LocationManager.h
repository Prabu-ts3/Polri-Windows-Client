#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QVariantList>

class LocationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant userLocations READ getUserLocations NOTIFY userLocationsChanged)
    Q_PROPERTY(double latitude READ getLatitude NOTIFY positionChanged)
    Q_PROPERTY(double longitude READ getLongitude NOTIFY positionChanged)
    Q_PROPERTY(double accuracy READ getAccuracy NOTIFY positionChanged)

public:
    explicit LocationManager(QObject *parent = nullptr);
    static LocationManager* instance();

    QVariant getUserLocations() const { return QVariant::fromValue(m_userLocations); }
    double getLatitude() const { return m_latitude; }
    double getLongitude() const { return m_longitude; }
    double getAccuracy() const { return m_accuracy; }

    void updateRemoteUsers(const QJsonArray &users);

signals:
    void userLocationsChanged();
    void positionChanged();

private slots:
    void positionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *m_source = nullptr;
    QVariantList m_userLocations;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    double m_accuracy = 0.0;
};

#endif // LOCATIONMANAGER_H
