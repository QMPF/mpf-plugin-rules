#include "demo_service.h"
#include <mpf/http/http_client.h>
#include <mpf/logger.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDateTime>

namespace rules {

DemoService::DemoService(const QString& pluginId, QObject* parent)
    : QObject(parent)
    , m_pluginId(pluginId)
{
    m_httpClient = std::make_unique<mpf::http::HttpClient>(this);
}

DemoService::~DemoService() = default;

// =============================================================================
// HTTP Demo
// =============================================================================

void DemoService::testGet(const QString& url)
{
    MPF_LOG_INFO("DemoService", QString("GET %1").arg(url).toStdString().c_str());

    m_requestTimer.start();

    auto* reply = m_httpClient->get(QUrl(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int elapsed = static_cast<int>(m_requestTimer.elapsed());
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        bool success = (reply->error() == QNetworkReply::NoError);
        QString body;

        if (success) {
            body = QString::fromUtf8(reply->readAll());
        } else {
            body = QString("Error: %1\n%2").arg(reply->errorString(), QString::fromUtf8(reply->readAll()));
        }

        emit httpResponseReceived(success, statusCode, body, elapsed);
        reply->deleteLater();
    });
}

void DemoService::testPost(const QString& url, const QString& jsonBody)
{
    MPF_LOG_INFO("DemoService", QString("POST %1").arg(url).toStdString().c_str());

    m_requestTimer.start();

    // Parse JSON body
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonBody.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit httpResponseReceived(false, 0,
            QString("JSON parse error: %1").arg(parseError.errorString()), 0);
        return;
    }

    auto* reply = m_httpClient->postJson(QUrl(url), doc.object());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        int elapsed = static_cast<int>(m_requestTimer.elapsed());
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        bool success = (reply->error() == QNetworkReply::NoError);
        QString body;

        if (success) {
            body = QString::fromUtf8(reply->readAll());
        } else {
            body = QString("Error: %1\n%2").arg(reply->errorString(), QString::fromUtf8(reply->readAll()));
        }

        emit httpResponseReceived(success, statusCode, body, elapsed);
        reply->deleteLater();
    });
}

// =============================================================================
// EventBus Message Accumulation
// =============================================================================

QVariantList DemoService::receivedMessages() const
{
    return m_receivedMessages;
}

void DemoService::clearMessages()
{
    m_receivedMessages.clear();
    emit messagesChanged();
}

int DemoService::messageCount() const
{
    return m_receivedMessages.size();
}

void DemoService::connectToEventBus(QObject* eventBusObj, const QString& topicPrefix)
{
    m_topicPrefix = topicPrefix;

    // Use old-style connect for cross-DLL safety
    // EventBusService emits: eventPublished(QString, QVariantMap, QString)
    connect(eventBusObj, SIGNAL(eventPublished(QString,QVariantMap,QString)),
            this, SLOT(onEventReceived(QString,QVariantMap,QString)));

    // Also register a pattern subscription so the EventBus emits the signal
    // (deliverEvent skips signal emission when no pattern subscribers match)
    QString pattern = topicPrefix + "**";
    QString subId;
    QMetaObject::invokeMethod(eventBusObj, "subscribeSimple",
        Q_RETURN_ARG(QString, subId),
        Q_ARG(QString, pattern),
        Q_ARG(QString, m_pluginId + ".demo"));

    MPF_LOG_INFO("DemoService",
        QString("Connected to EventBus, filtering: %1").arg(topicPrefix).toStdString().c_str());
}

void DemoService::onEventReceived(const QString& topic, const QVariantMap& data,
                                   const QString& senderId)
{
    // Filter by topic prefix
    if (!topic.startsWith(m_topicPrefix)) {
        return;
    }

    // Don't receive our own events
    if (senderId == m_pluginId) {
        return;
    }

    QVariantMap msg;
    msg["topic"] = topic;
    msg["data"] = data;
    msg["senderId"] = senderId;
    msg["timestamp"] = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    msg["message"] = data.value("message", "").toString();

    // Prepend (newest first)
    m_receivedMessages.prepend(msg);

    // Cap at MAX_MESSAGES
    while (m_receivedMessages.size() > MAX_MESSAGES) {
        m_receivedMessages.removeLast();
    }

    emit messagesChanged();

    MPF_LOG_DEBUG("DemoService",
        QString("Received event: %1 from %2").arg(topic, senderId).toStdString().c_str());
}

} // namespace rules
