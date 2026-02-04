#include "order_model.h"
#include "orders_service.h"

namespace orders {

OrderModel::OrderModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

OrderModel::OrderModel(OrdersService* service, QObject* parent)
    : QAbstractListModel(parent)
{
    setService(service);
}

OrderModel::~OrderModel() = default;

int OrderModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) return 0;
    return m_filteredOrders.size();
}

QVariant OrderModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_filteredOrders.size()) {
        return QVariant();
    }

    const QVariantMap& order = m_filteredOrders.at(index.row()).toMap();

    switch (role) {
    case IdRole:
        return order.value("id");
    case CustomerNameRole:
        return order.value("customerName");
    case ProductNameRole:
        return order.value("productName");
    case QuantityRole:
        return order.value("quantity");
    case PriceRole:
        return order.value("price");
    case StatusRole:
        return order.value("status");
    case CreatedAtRole:
        return order.value("createdAt");
    case UpdatedAtRole:
        return order.value("updatedAt");
    case TotalRole:
        return order.value("total");
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> OrderModel::roleNames() const
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

void OrderModel::setFilterStatus(const QString& status)
{
    if (m_filterStatus != status) {
        m_filterStatus = status;
        updateFilteredOrders();
        emit filterStatusChanged();
    }
}

void OrderModel::setService(OrdersService* service)
{
    if (m_service == service) {
        return;
    }

    if (m_service) {
        disconnect(m_service, nullptr, this, nullptr);
    }

    m_service = service;

    if (m_service) {
        connect(m_service, &OrdersService::ordersChanged, this, &OrderModel::onOrdersChanged);
    }

    updateFilteredOrders();
    emit serviceChanged();
}

void OrderModel::refresh()
{
    updateFilteredOrders();
}

QVariantMap OrderModel::get(int index) const
{
    if (index < 0 || index >= m_filteredOrders.size()) {
        return {};
    }
    return m_filteredOrders.at(index).toMap();
}

void OrderModel::onOrdersChanged()
{
    updateFilteredOrders();
}

void OrderModel::updateFilteredOrders()
{
    beginResetModel();

    if (!m_service) {
        m_filteredOrders = {};
    } else if (m_filterStatus.isEmpty()) {
        m_filteredOrders = m_service->getAllOrders();
    } else {
        m_filteredOrders = m_service->getOrdersByStatus(m_filterStatus);
    }
    
    endResetModel();
    emit countChanged();
}

} // namespace orders
