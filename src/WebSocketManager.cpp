#include "WebSocketManager.h"
#include "AudioHandler.h"
#include "VideoRenderer.h"
#include "SettingsManager.h"
#include "LocationManager.h"
#include "SoundManager.h"
#include <QDebug>
#include <QtEndian>
#include <QDateTime>
#include <QJsonArray>

static WebSocketManager* s_instance = nullptr;

WebSocketManager::WebSocketManager(QObject *parent) : QObject(parent)
{
    s_instance = this;

    m_idleTimer = new QTimer(this);
    connect(m_idleTimer, &QTimer::timeout, this, &WebSocketManager::checkSpeakerIdle);
    m_idleTimer->start(150);

    m_pttDelayTimer = new QTimer(this);
    m_pttDelayTimer->setSingleShot(true);
    connect(m_pttDelayTimer, &QTimer::timeout, this, &WebSocketManager::processStartTalking);

    m_ptpTimeoutTimer = new QTimer(this);
    m_ptpTimeoutTimer->setSingleShot(true);
    connect(m_ptpTimeoutTimer, &QTimer::timeout, this, [this]() {
        if (m_ptpStatus == PtpStatusRequesting) {
            m_ptpStatus = PtpStatusTimeout;
            emit ptpHandshakeChanged();
            updateTalkingStatusUI();
        }
    });

    connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketManager::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketManager::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketManager::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &WebSocketManager::onBinaryMessageReceived);

    AudioHandler* audio = AudioHandler::instance();

    connect(audio, &AudioHandler::audioDataReady, this, [this](const QByteArray &data) {
        if (m_isConnected && (m_talkingStatus.contains("Speaking") || m_talkingStatus.contains("Private to") || SettingsManager::instance()->isGatewayMode())) {
            QByteArray packet;
            packet.append(static_cast<char>(1)); // Type 1: Audio

            char idBytes[4];
            qToLittleEndian(m_myIdTruncated, idBytes);
            packet.append(idBytes, 4);

            packet.append(data);
            m_webSocket.sendBinaryMessage(packet);
        }
    });

    connect(audio, &AudioHandler::voxTriggered, this, [this](bool speaking) {
        if (SettingsManager::instance()->isVoxEnabled()) {
            if (speaking) {
                startTalking();
            } else {
                stopTalking();
            }
        }
    });

    connect(SettingsManager::instance(), &SettingsManager::serverUrlChanged, this, [this]() {
        if (!m_savedUsername.isEmpty()) {
            qDebug() << "Server URL updated, reconnecting...";
            m_webSocket.close();
            login(m_savedUsername, m_savedPassword, SettingsManager::instance()->getServerUrl());
        }
    });
}

WebSocketManager* WebSocketManager::instance()
{
    return s_instance;
}

void WebSocketManager::login(const QString &username, const QString &password, const QString &serverUrl)
{
    m_savedUsername = username;
    m_savedPassword = password;

    m_connectionStatus = StatusConnecting;
    emit connectionStatusChanged();

    m_webSocket.open(QUrl(serverUrl));
    qDebug() << "Connecting to:" << serverUrl;
}

void WebSocketManager::onConnected()
{
    qDebug() << "WebSocket Connected";
    m_isConnected = true;
    m_connectionStatus = StatusConnected;
    m_commState = CommStateIdle;
    m_reconnectInterval = 2000; // Reset backoff on success
    emit connectionStatusChanged();
    emit communicationStateChanged();

    QJsonObject authData;
    authData["username"] = m_savedUsername;
    authData["password"] = m_savedPassword;
    authData["current_device_id"] = "WINDOWS_CLIENT";

    sendJson("app_login", authData);
}

void WebSocketManager::onDisconnected()
{
    qDebug() << "WebSocket Disconnected";
    m_isConnected = false;
    m_connectionStatus = StatusDisconnected;
    m_commState = CommStateOffline;
    emit connectionStatusChanged();
    emit communicationStateChanged();

    m_activeSpeakers.clear();
    m_speakerLastSeen.clear();
    updateTalkingStatusUI();

    // Exponential Backoff Reconnection (Parity with Android)
    QTimer::singleShot(m_reconnectInterval, this, [this]() {
        if (m_connectionStatus == StatusDisconnected && !m_savedUsername.isEmpty()) {
            qDebug() << "Attempting to reconnect in" << m_reconnectInterval << "ms";
            m_webSocket.open(m_webSocket.requestUrl());

            // Increase interval for next time
            m_reconnectInterval = qMin(m_reconnectInterval + 2000, MAX_RECONNECT_INTERVAL);
        }
    });
}

void WebSocketManager::onTextMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        handleJsonMessage(doc.object());
    }
}

void WebSocketManager::handleJsonMessage(const QJsonObject &root)
{
    QString type = root["type"].toString();
    QJsonObject data = root["data"].toObject();

    if (type == "login_success") {
        m_myUserId = data["id"].toString();
        m_myUserName = data["username"].toString();
        m_myIdTruncated = toTruncatedId(m_myUserId);

        m_isRxOnly = data["is_rx_only"].toBool();
        m_isVideoEnabled = data["enable_ptt_video"].toBool();
        m_isPtpEnabled = data["enable_p2p"].toBool(true);
        m_duplexMode = data["duplex_mode"].toString("HALF DUPLEX");

        if (data.contains("channels")) {
            m_channelsList = data["channels"].toArray().toVariantList();
            emit channelsListChanged();
        }

        emit permissionsChanged();
        emit loginSuccess();

        updateTalkingStatusUI();
    } else if (type == "login_error") {
        emit loginError(data["message"].toString());
    } else if (type == "channels_updated") {
        m_channelsList = data["channels"].toArray().toVariantList();
        emit channelsListChanged();
    } else if (type == "join_channel_success") {
        m_channelName = data["channel_name"].toString();
        m_lastChannelName = m_channelName;

        QJsonArray speakers = data["speakers"].toArray();
        m_activeSpeakers.clear();
        for (const QJsonValue &v : speakers) {
            QString name = v.toString();
            if (name != m_myUserName) {
                m_activeSpeakers.insert(name);
                m_speakerLastSeen[name] = QDateTime::currentMSecsSinceEpoch();
            }
        }

        if (m_ptpTargetId.isEmpty()) {
            emit channelNameChanged();
        }
        updateTalkingStatusUI();
    } else if (type == "permission_update") {
        if (data.contains("is_rx_only")) m_isRxOnly = data["is_rx_only"].toBool();
        if (data.contains("enable_ptt_video")) m_isVideoEnabled = data["enable_ptt_video"].toBool();
        if (data.contains("enable_p2p")) m_isPtpEnabled = data["enable_p2p"].toBool();
        if (data.contains("duplex_mode")) m_duplexMode = data["duplex_mode"].toString();
        emit permissionsChanged();
        updateTalkingStatusUI();
    } else if (type == "ptp_invitation") {
        QString senderId = data["sender_id"].toString();
        QString senderName = data["sender_name"].toString();
        if (m_ptpTargetId.isEmpty()) {
            m_ptpTargetId = senderId;
            m_ptpTargetName = senderName;
            m_channelName = "PRIVATE WITH " + m_ptpTargetName;
            emit channelNameChanged();
            emit ptpTargetNameChanged();
            emit ptpStatusChanged();
            sendJson("accept_ptp", QJsonObject{{"target_id", senderId}});
        }
    } else if (type == "ptp_confirmed") {
        m_ptpTimeoutTimer->stop();
        m_ptpStatus = PtpStatusAccepted;
        emit ptpHandshakeChanged();
        m_ptpTargetId = data["target_id"].toString();
        m_ptpTargetName = data["target_name"].toString();
        m_channelName = "PRIVATE WITH " + m_ptpTargetName;
        emit channelNameChanged();
        emit ptpTargetNameChanged();
        emit ptpStatusChanged();
    } else if (type == "ptp_failed") {
        m_ptpTimeoutTimer->stop();
        m_ptpStatus = PtpStatusFailed;
        emit ptpHandshakeChanged();
    } else if (type == "ptp_cancelled") {
        m_ptpStatus = PtpStatusNone;
        emit ptpHandshakeChanged();
        m_ptpTargetId = "";
        m_ptpTargetName = "";
        m_channelName = m_lastChannelName;
        emit channelNameChanged();
        emit ptpTargetNameChanged();
        emit ptpStatusChanged();
    } else if (type == "ptt_active_status") {
        m_isPrivateRx = data["is_private"].toBool();
        QJsonArray speakers = data["speakers"].toArray();
        m_activeSpeakers.clear();
        for (const QJsonValue &v : speakers) {
            QString name = v.toString();
            if (name != m_myUserName) {
                m_activeSpeakers.insert(name);
                m_speakerLastSeen[name] = QDateTime::currentMSecsSinceEpoch();
            }
        }
        updateTalkingStatusUI();
    } else if (type == "users_online") {
        QJsonArray usersArray = data["users"].toArray();
        m_usersOnline = usersArray.toVariantList();
        m_userIdToNameMap.clear();
        for (const QJsonValue &v : usersArray) {
            QJsonObject u = v.toObject();
            m_userIdToNameMap[toTruncatedId(u["id"].toString())] = u["name"].toString();
        }
        emit usersOnlineChanged();
    } else if (type == "sos_signal_received") {
        QString senderId = data["sender_id"].toString();
        if (senderId == m_myUserId) return;

        QString senderName = data["sender_name"].toString();
        double lat = data["latitude"].toDouble();
        double lon = data["longitude"].toDouble();
        m_isSosActive = true;
        emit sosStatusChanged();
        emit sosReceived(senderName, lat, lon);
        SoundManager::instance()->startSosLoop();
    } else if (type == "sos_confirmed_broadcast") {
        m_isSosActive = false;
        emit sosStatusChanged();
        SoundManager::instance()->stopSosLoop();
    }
}

void WebSocketManager::onBinaryMessageReceived(const QByteArray &data)
{
    if (data.size() < 5) return;

    int type = static_cast<int>(data[0]);
    quint32 userIdTruncated = qFromLittleEndian<quint32>(data.constData() + 1);
    QByteArray payload = data.mid(5);

    if (userIdTruncated == m_myIdTruncated) return;

    QString senderName = findUserNameById(userIdTruncated);
    if (senderName.isEmpty()) senderName = QString("User-%1").arg(userIdTruncated);

    if (type == 1) { // Audio
        if (m_talkingStatus == "IDLE" || m_talkingStatus.contains("Speaking")) {
            if (!m_activeSpeakers.contains(senderName)) {
                m_activeSpeakers.insert(senderName);
                updateTalkingStatusUI();
            }
            m_speakerLastSeen[senderName] = QDateTime::currentMSecsSinceEpoch();
        }
        AudioHandler::instance()->playAudio(userIdTruncated, payload);
    } else if (type == 2) { // Video
        VideoRenderer::instance()->processFrame(payload);
    }
}

void WebSocketManager::checkSpeakerIdle()
{
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    bool changed = false;
    QMutableSetIterator<QString> it(m_activeSpeakers);
    while (it.hasNext()) {
        QString name = it.next();
        if (now - m_speakerLastSeen.value(name, 0) > 1000) {
            it.remove();
            m_speakerLastSeen.remove(name);
            changed = true;
        }
    }
    if (changed) {
        updateTalkingStatusUI();
    }
}

void WebSocketManager::updateTalkingStatusUI()
{
    QString nextStatus = "IDLE";
    CommState nextCommState = CommStateIdle;
    bool isMeTalking = m_talkingStatus == "You are Speaking" || m_talkingStatus.startsWith("Private to");

    if (isMeTalking) {
        nextCommState = CommStateTx;
        if (!m_ptpTargetId.isEmpty()) nextStatus = "Private to " + m_ptpTargetName;
        else nextStatus = "You are Speaking";
    } else if (!m_activeSpeakers.isEmpty()) {
        nextCommState = CommStateRx;
        QStringList names = m_activeSpeakers.values();
        QString combined = names.join(", ");
        if (m_isPrivateRx) nextStatus = "Private from " + combined;
        else nextStatus = combined + (names.size() > 1 ? " are Speaking" : " is Speaking");
        m_currentReceiverName = names.first();
    } else {
        if (m_talkingStatus.contains("Speaking") || m_talkingStatus.contains("Private")) {
            QString time = QDateTime::currentDateTime().toString("HH:mm");
            QString who = isMeTalking ? (m_myUserName.isEmpty() ? "You" : m_myUserName) : (m_currentReceiverName.isEmpty() ? "Rekan" : m_currentReceiverName);
            m_lastSpeaker = QString("Terakhir: %1 @ %2").arg(who, time);
            emit lastSpeakerChanged();
        }
        nextStatus = "IDLE";
        nextCommState = CommStateIdle;
    }

    if (m_commState != nextCommState) {
        m_commState = nextCommState;
        emit communicationStateChanged();
    }

    if (m_talkingStatus != nextStatus) {
        if (nextStatus.contains("Speaking") || nextStatus.contains("Private from")) {
            if (m_talkingStatus == "IDLE") SoundManager::instance()->playRxStart();
        } else if (nextStatus == "IDLE") {
            if (m_talkingStatus.contains("Speaking") && !m_talkingStatus.contains("You are")) {
                 SoundManager::instance()->playRxStop();
            }
        }
        m_talkingStatus = nextStatus;
        emit talkingStatusChanged();
    }
}

void WebSocketManager::startTalking()
{
    if (!m_isConnected || (m_isRxOnly && m_ptpTargetId.isEmpty())) return;
    if (m_duplexMode == "HALF DUPLEX" && !m_activeSpeakers.isEmpty() && m_ptpTargetId.isEmpty()) return;

    m_pttDelayTimer->start(800);
    SoundManager::instance()->playStartTx();
}

void WebSocketManager::processStartTalking()
{
    if (!m_isConnected) return;

    if (!m_ptpTargetId.isEmpty()) {
        sendJson("ptt_audio_start_private", QJsonObject{{"target_id", m_ptpTargetId}});
        m_talkingStatus = "Private to " + m_ptpTargetName;
    } else {
        sendJson("ptt_audio_start", QJsonObject());
        m_talkingStatus = "You are Speaking";
    }

    AudioHandler::instance()->startRecording();
    updateTalkingStatusUI();
}

void WebSocketManager::stopTalking()
{
    m_pttDelayTimer->stop();
    AudioHandler::instance()->stopRecording();

    if (m_talkingStatus.contains("Speaking") || m_talkingStatus.contains("Private to")) {
        if (!m_ptpTargetId.isEmpty()) {
            sendJson("ptt_audio_end_private", QJsonObject{{"target_id", m_ptpTargetId}});
        } else {
            sendJson("ptt_audio_end", QJsonObject());
        }
    }

    SoundManager::instance()->playStopTx();
    m_talkingStatus = "IDLE";
    updateTalkingStatusUI();
}

void WebSocketManager::startPtp(const QString &userId, const QString &userName)
{
    if (m_ptpTargetId == userId || m_ptpStatus == PtpStatusRequesting) return;

    m_ptpStatus = PtpStatusRequesting;
    m_ptpTargetName = userName;
    emit ptpHandshakeChanged();

    m_ptpTimeoutTimer->start(10000);
    sendJson("request_ptp", QJsonObject{{"target_id", userId}});
}

void WebSocketManager::endPtp()
{
    m_ptpTimeoutTimer->stop();
    m_ptpStatus = PtpStatusNone;
    emit ptpHandshakeChanged();

    sendJson("cancel_ptp", QJsonObject());
    m_ptpTargetId = "";
    m_ptpTargetName = "";
    m_channelName = m_lastChannelName;
    emit channelNameChanged();
    emit ptpTargetNameChanged();
    emit ptpStatusChanged();
}

void WebSocketManager::sendSos()
{
    QJsonObject data;
    data["latitude"] = LocationManager::instance()->getLatitude();
    data["longitude"] = LocationManager::instance()->getLongitude();
    data["accuracy"] = (int)LocationManager::instance()->getAccuracy();
    data["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    sendJson("sos_signal", data);
}

void WebSocketManager::confirmSos(const QString &senderId)
{
    sendJson("confirm_sos", QJsonObject{{"sos_sender_id", senderId}});
}

void WebSocketManager::joinChannel(const QString &slug)
{
    sendJson("join_channel", QJsonObject{{"new_channel_slug", slug}});
}

void WebSocketManager::sendLocation(double lat, double lon, double acc, const QString &addr)
{
    QJsonObject data;
    data["latitude"] = lat;
    data["longitude"] = lon;
    data["accuracy"] = acc;
    data["address"] = addr;
    sendJson("update_location", data);
}

void WebSocketManager::sendJson(const QString &type, const QJsonObject &data)
{
    QJsonObject root;
    root["type"] = type;
    root["data"] = data;
    m_webSocket.sendTextMessage(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

quint32 WebSocketManager::toTruncatedId(const QString &userId)
{
    bool ok;
    qlonglong val = userId.toLongLong(&ok);
    if (ok) return static_cast<quint32>(val & 0xFFFFFFFFL);
    return static_cast<quint32>(qHash(userId));
}

QString WebSocketManager::findUserNameById(quint32 truncatedId)
{
    return m_userIdToNameMap.value(truncatedId, "");
}
