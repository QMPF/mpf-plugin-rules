#pragma once

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QDateTime>

namespace rules {

struct Rule {
    QString id;
    QString customerName;
    QString productName;
    int quantity;
    double price;
    QString status;  // pending, processing, shipped, delivered, cancelled
    QDateTime createdAt;
    QDateTime updatedAt;
    
    QVariantMap toVariantMap() const;
    static Rule fromVariantMap(const QVariantMap& map);
};

/**
 * @brief Rules business service
 * 
 * Provides rule management functionality.
 * This could be exposed as an interface if other plugins need it.
 */
class RulesService : public QObject
{
    Q_OBJECT

public:
    explicit RulesService(QObject* parent = nullptr);
    ~RulesService() override;

    // CRUD operations
    Q_INVOKABLE QVariantList getAllRules() const;
    Q_INVOKABLE QVariantMap getRule(const QString& id) const;
    Q_INVOKABLE QString createRule(const QVariantMap& data);
    Q_INVOKABLE bool updateRule(const QString& id, const QVariantMap& data);
    Q_INVOKABLE bool deleteRule(const QString& id);
    
    // Business operations
    Q_INVOKABLE bool updateStatus(const QString& id, const QString& status);
    Q_INVOKABLE QVariantList getRulesByStatus(const QString& status) const;
    Q_INVOKABLE int getRuleCount() const;
    Q_INVOKABLE double getTotalRevenue() const;

signals:
    void ruleCreated(const QString& id);
    void ruleUpdated(const QString& id);
    void ruleDeleted(const QString& id);
    void rulesChanged();

private:
    QString generateId() const;
    
    QList<Rule> m_rules;
};

} // namespace rules
