#ifndef LOCATIONMANAGER_H
#define LOCATIONMANAGER_H

#include <QObject>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QList>
#include <QMetaType>

class LocationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> userLocations READ userLocations NOTIFY userLocationsChanged)
    Q_PROPERTY(double latitude READ latitude NOTIFY positionChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY positionChanged)
    Q_PROPERTY(double accuracy READ accuracy NOTIFY positionChanged)

public:
    explicit LocationManager(QObject *parent = nullptr);
    static LocationManager* instance();

    QList<QVariant> userLocations() const { return m_userLocations; }
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double accuracy() const { return m_accuracy; }

    void updateRemoteUsers(const QJsonArray &users);

signals:
    void userLocationsChanged();
    void positionChanged();

private slots:
    void positionUpdated(const QGeoPositionInfo &info);

private:
    QGeoPositionInfoSource *m_source = nullptr;
    QList<QVariant> m_userLocations;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    double m_accuracy = 0.0;
};

Q_DECLARE_METATYPE(QList<QVariant>)

#endif // LOCATIONMANAGER_H
