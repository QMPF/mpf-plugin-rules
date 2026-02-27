#include "rules_service.h"
#include <QUuid>
#include <QDateTime>
#include <algorithm>

namespace rules {

// Rule methods

QVariantMap Rule::toVariantMap() const
{
    return {
        {"id", id},
        {"customerName", customerName},
        {"productName", productName},
        {"quantity", quantity},
        {"price", price},
        {"status", status},
        {"createdAt", createdAt},
        {"updatedAt", updatedAt},
        {"total", quantity * price}
    };
}

Rule Rule::fromVariantMap(const QVariantMap& map)
{
    Rule rule;
    rule.id = map.value("id").toString();
    rule.customerName = map.value("customerName").toString();
    rule.productName = map.value("productName").toString();
    rule.quantity = map.value("quantity").toInt();
    rule.price = map.value("price").toDouble();
    rule.status = map.value("status", "pending").toString();
    rule.createdAt = map.value("createdAt").toDateTime();
    rule.updatedAt = map.value("updatedAt").toDateTime();
    return rule;
}

// RulesService methods

RulesService::RulesService(QObject* parent)
    : QObject(parent)
{
}

RulesService::~RulesService() = default;

QVariantList RulesService::getAllRules() const
{
    QVariantList result;
    for (const Rule& rule : m_rules) {
        result.append(rule.toVariantMap());
    }
    return result;
}

QVariantMap RulesService::getRule(const QString& id) const
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [&id](const Rule& o) { return o.id == id; });
    
    if (it != m_rules.end()) {
        return it->toVariantMap();
    }
    return {};
}

QString RulesService::createRule(const QVariantMap& data)
{
    Rule rule = Rule::fromVariantMap(data);
    rule.id = generateId();
    rule.createdAt = QDateTime::currentDateTime();
    rule.updatedAt = rule.createdAt;
    
    if (rule.status.isEmpty()) {
        rule.status = "pending";
    }
    
    m_rules.append(rule);
    
    emit ruleCreated(rule.id);
    emit rulesChanged();
    
    return rule.id;
}

bool RulesService::updateRule(const QString& id, const QVariantMap& data)
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [&id](const Rule& o) { return o.id == id; });
    
    if (it == m_rules.end()) {
        return false;
    }
    
    if (data.contains("customerName")) it->customerName = data["customerName"].toString();
    if (data.contains("productName")) it->productName = data["productName"].toString();
    if (data.contains("quantity")) it->quantity = data["quantity"].toInt();
    if (data.contains("price")) it->price = data["price"].toDouble();
    if (data.contains("status")) it->status = data["status"].toString();
    it->updatedAt = QDateTime::currentDateTime();
    
    emit ruleUpdated(id);
    emit rulesChanged();
    
    return true;
}

bool RulesService::deleteRule(const QString& id)
{
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [&id](const Rule& o) { return o.id == id; });
    
    if (it == m_rules.end()) {
        return false;
    }
    
    m_rules.erase(it);
    
    emit ruleDeleted(id);
    emit rulesChanged();
    
    return true;
}

bool RulesService::updateStatus(const QString& id, const QString& status)
{
    return updateRule(id, {{"status", status}});
}

QVariantList RulesService::getRulesByStatus(const QString& status) const
{
    QVariantList result;
    for (const Rule& rule : m_rules) {
        if (rule.status == status) {
            result.append(rule.toVariantMap());
        }
    }
    return result;
}

int RulesService::getRuleCount() const
{
    return m_rules.size();
}

double RulesService::getTotalRevenue() const
{
    double total = 0;
    for (const Rule& rule : m_rules) {
        total += rule.quantity * rule.price;
    }
    return total;
}

QString RulesService::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

} // namespace rules
