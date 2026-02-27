#pragma once

#include <QAbstractListModel>
#include "rules_service.h"

namespace rules {

/**
 * @brief List model for rules
 * 
 * Exposes rules to QML ListView/Repeater.
 */
class RuleModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString filterStatus READ filterStatus WRITE setFilterStatus NOTIFY filterStatusChanged)
    Q_PROPERTY(rules::RulesService* service READ service WRITE setService NOTIFY serviceChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        CustomerNameRole,
        ProductNameRole,
        QuantityRole,
        PriceRole,
        StatusRole,
        CreatedAtRole,
        UpdatedAtRole,
        TotalRole
    };

    explicit RuleModel(QObject* parent = nullptr);
    explicit RuleModel(RulesService* service, QObject* parent = nullptr);
    ~RuleModel() override;
    RulesService* service() const { return m_service; }
    void setService(RulesService* service);


    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Filter
    QString filterStatus() const { return m_filterStatus; }
    void setFilterStatus(const QString& status);

    // Actions
    Q_INVOKABLE void refresh();
    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void countChanged();
    void filterStatusChanged();
    void serviceChanged();

private slots:
    void onRulesChanged();

private:
    void updateFilteredRules();

    RulesService* m_service = nullptr;
    QVariantList m_filteredRules;
    QString m_filterStatus;
};

} // namespace rules
