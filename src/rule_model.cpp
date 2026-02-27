#include "rule_model.h"
#include "rules_service.h"

namespace rules {

RuleModel::RuleModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

RuleModel::RuleModel(RulesService* service, QObject* parent)
    : QAbstractListModel(parent)
{
    setService(service);
}

RuleModel::~RuleModel() = default;

int RuleModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_filteredRules.size();
}

QVariant RuleModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredRules.size()) {
        return QVariant();
    }

    const QVariantMap& rule = m_filteredRules.at(index.row()).toMap();

    switch (role) {
    case IdRole:
        return rule.value("id");
    case CustomerNameRole:
        return rule.value("customerName");
    case ProductNameRole:
        return rule.value("productName");
    case QuantityRole:
        return rule.value("quantity");
    case PriceRole:
        return rule.value("price");
    case StatusRole:
        return rule.value("status");
    case CreatedAtRole:
        return rule.value("createdAt");
    case UpdatedAtRole:
        return rule.value("updatedAt");
    case TotalRole:
        return rule.value("total");
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> RuleModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {CustomerNameRole, "customerName"},
        {ProductNameRole, "productName"},
        {QuantityRole, "quantity"},
        {PriceRole, "price"},
        {StatusRole, "status"},
        {CreatedAtRole, "createdAt"},
        {UpdatedAtRole, "updatedAt"},
        {TotalRole, "total"}
    };
}

void RuleModel::setFilterStatus(const QString& status)
{
    if (m_filterStatus != status) {
        m_filterStatus = status;
        updateFilteredRules();
        emit filterStatusChanged();
    }
}

void RuleModel::setService(RulesService* service)
{
    if (m_service == service) {
        return;
    }

    if (m_service) {
        disconnect(m_service, nullptr, this, nullptr);
    }

    m_service = service;

    if (m_service) {
        connect(m_service, &RulesService::rulesChanged, this, &RuleModel::onRulesChanged);
    }

    updateFilteredRules();
    emit serviceChanged();
}

void RuleModel::refresh()
{
    updateFilteredRules();
}

QVariantMap RuleModel::get(int index) const
{
    if (index < 0 || index >= m_filteredRules.size()) {
        return {};
    }
    return m_filteredRules.at(index).toMap();
}

void RuleModel::onRulesChanged()
{
    updateFilteredRules();
}

void RuleModel::updateFilteredRules()
{
    beginResetModel();

    if (!m_service) {
        m_filteredRules = {};
    } else if (m_filterStatus.isEmpty()) {
        m_filteredRules = m_service->getAllRules();
    } else {
        m_filteredRules = m_service->getRulesByStatus(m_filterStatus);
    }
    
    endResetModel();
    emit countChanged();
}

} // namespace rules
