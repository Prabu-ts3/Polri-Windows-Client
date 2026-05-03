#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QSet>
#include <QMap>
#include <QVariant>
#include <QList>
#include <QMetaType>

class WebSocketManager : public QObject
{
    Q_OBJECT

public:
    enum ConnectionStatus {
        StatusDisconnected,
        StatusConnecting,
        StatusConnected
    };
    Q_ENUM(ConnectionStatus)

    enum CommState {
        CommStateOffline,
        CommStateReconnecting,
        CommStateIdle,
        CommStateTx,
        CommStateRx,
        CommStateCalling
    };
    Q_ENUM(CommState)

    enum PtpHandshakeStatus {
        PtpStatusNone,
        PtpStatusRequesting,
        PtpStatusAccepted,
        PtpStatusFailed,
        PtpStatusTimeout
    };
    Q_ENUM(PtpHandshakeStatus)

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString channelName READ channelName NOTIFY channelNameChanged)
    Q_PROPERTY(QString talkingStatus READ talkingStatus NOTIFY talkingStatusChanged)
    Q_PROPERTY(QString lastSpeaker READ lastSpeaker NOTIFY lastSpeakerChanged)
    Q_PROPERTY(QString ptpTargetName READ ptpTargetName NOTIFY ptpTargetNameChanged)
    Q_PROPERTY(QString myUserId READ myUserId NOTIFY myUserIdChanged)
    Q_PROPERTY(bool isPtpActive READ isPtpActive NOTIFY ptpStatusChanged)
    Q_PROPERTY(bool isRxOnly READ isRxOnly NOTIFY permissionsChanged)
    Q_PROPERTY(bool isVideoEnabled READ isVideoEnabled NOTIFY permissionsChanged)
    Q_PROPERTY(bool isPtpEnabled READ isPtpEnabled NOTIFY permissionsChanged)
    Q_PROPERTY(QString duplexMode READ duplexMode NOTIFY permissionsChanged)
    Q_PROPERTY(QVariantList usersOnline READ usersOnline NOTIFY usersOnlineChanged)
    Q_PROPERTY(QVariantList channelsList READ channelsList NOTIFY channelsListChanged)
    Q_PROPERTY(ConnectionStatus connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool isSosActive READ isSosActive NOTIFY sosStatusChanged)
    Q_PROPERTY(CommState communicationState READ communicationState NOTIFY communicationStateChanged)
    Q_PROPERTY(PtpHandshakeStatus ptpHandshakeStatus READ ptpHandshakeStatus NOTIFY ptpHandshakeChanged)

    explicit WebSocketManager(QObject *parent = nullptr);
    static WebSocketManager* instance();

    bool isConnected() const { return m_isConnected; }
    ConnectionStatus connectionStatus() const { return m_connectionStatus; }
    CommState communicationState() const { return m_commState; }
    PtpHandshakeStatus ptpHandshakeStatus() const { return m_ptpStatus; }

    QString channelName() const { return m_channelName; }
    QString talkingStatus() const { return m_talkingStatus; }
    QString lastSpeaker() const { return m_lastSpeaker; }
    QString ptpTargetName() const { return m_ptpTargetId.isEmpty() ? "" : m_ptpTargetName; }
    QString myUserId() const { return m_myUserId; }
    bool isPtpActive() const { return !m_ptpTargetId.isEmpty(); }
    bool isRxOnly() const { return m_isRxOnly; }
    bool isVideoEnabled() const { return m_isVideoEnabled; }
    bool isPtpEnabled() const { return m_isPtpEnabled; }
    QString duplexMode() const { return m_duplexMode; }
    QVariantList usersOnline() const { return m_usersOnline; }
    QVariantList channelsList() const { return m_channelsList; }
    bool isSosActive() const { return m_isSosActive; }

    Q_INVOKABLE void login(const QString &username, const QString &password, const QString &serverUrl);
    Q_INVOKABLE void startTalking();
    Q_INVOKABLE void stopTalking();
    Q_INVOKABLE void joinChannel(const QString &slug);
    Q_INVOKABLE void sendLocation(double lat, double lon, double acc, const QString &addr);
    Q_INVOKABLE void startPtp(const QString &userId, const QString &userName);
    Q_INVOKABLE void endPtp();
    Q_INVOKABLE void sendSos();
    Q_INVOKABLE void confirmSos(const QString &senderId);

signals:
    void connectedChanged();
    void connectionStatusChanged();
    void channelNameChanged();
    void talkingStatusChanged();
    void lastSpeakerChanged();
    void ptpTargetNameChanged();
    void ptpStatusChanged();
    void myUserIdChanged();
    void permissionsChanged();
    void sosStatusChanged();
    void communicationStateChanged();
    void ptpHandshakeChanged();
    void sosReceived(const QString &senderName, double lat, double lon);
    void loginSuccess();
    void loginError(const QString &message);
    void usersOnlineChanged();
    void channelsListChanged();

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &data);
    void checkSpeakerIdle();
    void processStartTalking();

private:
    void handleJsonMessage(const QJsonObject &root);
    void sendJson(const QString &type, const QJsonObject &data);
    void updateTalkingStatusUI();
    quint32 toTruncatedId(const QString &userId);
    QString findUserNameById(quint32 truncatedId);

    QWebSocket m_webSocket;
    bool m_isConnected = false;
    QString m_channelName = "Connecting...";
    QString m_lastChannelName = "Connecting...";
    QString m_talkingStatus = "IDLE";
    QString m_lastSpeaker = "Terakhir: -";
    QString m_myUserId;
    QString m_myUserName;
    quint32 m_myIdTruncated = 0;
    QString m_ptpTargetId;
    QString m_ptpTargetName;

    bool m_isRxOnly = false;
    bool m_isVideoEnabled = false;
    bool m_isPtpEnabled = true;
    QString m_duplexMode = "HALF DUPLEX";

    bool m_isSosActive = false;
    QString m_sosSenderId;

    ConnectionStatus m_connectionStatus = StatusDisconnected;
    CommState m_commState = CommStateOffline;
    PtpHandshakeStatus m_ptpStatus = PtpStatusNone;
    QTimer *m_ptpTimeoutTimer;

    QVariantList m_usersOnline;
    QVariantList m_channelsList;
    QString m_savedUsername;
    QString m_savedPassword;

    QSet<QString> m_activeSpeakers;
    QMap<QString, qint64> m_speakerLastSeen;
    QTimer *m_idleTimer;
    QTimer *m_pttDelayTimer;
    QString m_currentReceiverName;
    bool m_isPrivateRx = false;
    QMap<quint32, QString> m_userIdToNameMap;

    int m_reconnectInterval = 2000;
    const int MAX_RECONNECT_INTERVAL = 10000;
};

#endif // WEBSOCKETMANAGER_H
