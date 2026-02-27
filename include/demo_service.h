#pragma once

#include <QObject>
#include <QVariantList>
#include <QElapsedTimer>
#include <memory>

namespace mpf::http { class HttpClient; }

namespace rules {

/**
 * @brief Demo service for showcasing HTTP client and EventBus capabilities
 *
 * Provides Q_INVOKABLE methods for QML to:
 * - Send HTTP GET/POST requests via mpf::http::HttpClient
 * - Accumulate received EventBus messages for display
 */
class DemoService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList receivedMessages READ receivedMessages NOTIFY messagesChanged)

public:
    explicit DemoService(const QString& pluginId, QObject* parent = nullptr);
    ~DemoService() override;

    // HTTP demo
    Q_INVOKABLE void testGet(const QString& url);
    Q_INVOKABLE void testPost(const QString& url, const QString& jsonBody);

    // EventBus message accumulation
    QVariantList receivedMessages() const;
    Q_INVOKABLE void clearMessages();
    Q_INVOKABLE int messageCount() const;

    // Connect to EventBus signal for persistent listening
    void connectToEventBus(QObject* eventBusObj, const QString& topicPrefix);

signals:
    void httpResponseReceived(bool success, int statusCode,
                              const QString& body, int elapsedMs);
    void messagesChanged();

public slots:
    void onEventReceived(const QString& topic, const QVariantMap& data,
                         const QString& senderId);

private:
    std::unique_ptr<mpf::http::HttpClient> m_httpClient;
    QVariantList m_receivedMessages;
    QString m_pluginId;
    QString m_topicPrefix;
    QElapsedTimer m_requestTimer;

    static constexpr int MAX_MESSAGES = 50;
};

} // namespace rules
