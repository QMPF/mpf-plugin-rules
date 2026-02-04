#include "orders_service.h"
#include <QUuid>
#include <QDateTime>
#include <algorithm>

namespace orders {

// Order methods

QVariantMap Order::toVariantMap() const
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

Order Order::fromVariantMap(const QVariantMap& map)
{
    Order order;
    order.id = map.value("id").toString();
    order.customerName = map.value("customerName").toString();
    order.productName = map.value("productName").toString();
    order.quantity = map.value("quantity").toInt();
    order.price = map.value("price").toDouble();
    order.status = map.value("status", "pending").toString();
    order.createdAt = map.value("createdAt").toDateTime();
    order.updatedAt = map.value("updatedAt").toDateTime();
    return order;
}

// OrdersService methods

OrdersService::OrdersService(QObject* parent)
    : QObject(parent)
{
}

OrdersService::~OrdersService() = default;

QVariantList OrdersService::getAllOrders() const
{
    QVariantList result;
    for (const Order& order : m_orders) {
        result.append(order.toVariantMap());
    }
    return result;
}

QVariantMap OrdersService::getOrder(const QString& id) const
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it != m_orders.end()) {
        return it->toVariantMap();
    }
    return {};
}

QString OrdersService::createOrder(const QVariantMap& data)
{
    Order order = Order::fromVariantMap(data);
    order.id = generateId();
    order.createdAt = QDateTime::currentDateTime();
    order.updatedAt = order.createdAt;
    
    if (order.status.isEmpty()) {
        order.status = "pending";
    }
    
    m_orders.append(order);
    
    emit orderCreated(order.id);
    emit ordersChanged();
    
    return order.id;
}

bool OrdersService::updateOrder(const QString& id, const QVariantMap& data)
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it == m_orders.end()) {
        return false;
    }
    
    if (data.contains("customerName")) it->customerName = data["customerName"].toString();
    if (data.contains("productName")) it->productName = data["productName"].toString();
    if (data.contains("quantity")) it->quantity = data["quantity"].toInt();
    if (data.contains("price")) it->price = data["price"].toDouble();
    if (data.contains("status")) it->status = data["status"].toString();
    it->updatedAt = QDateTime::currentDateTime();
    
    emit orderUpdated(id);
    emit ordersChanged();
    
    return true;
}

bool OrdersService::deleteOrder(const QString& id)
{
    auto it = std::find_if(m_orders.begin(), m_orders.end(),
        [&id](const Order& o) { return o.id == id; });
    
    if (it == m_orders.end()) {
        return false;
    }
    
    m_orders.erase(it);
    
    emit orderDeleted(id);
    emit ordersChanged();
    
    return true;
}

bool OrdersService::updateStatus(const QString& id, const QString& status)
{
    return updateOrder(id, {{"status", status}});
}

QVariantList OrdersService::getOrdersByStatus(const QString& status) const
{
    QVariantList result;
    for (const Order& order : m_orders) {
        if (order.status == status) {
            result.append(order.toVariantMap());
        }
    }
    return result;
}

int OrdersService::getOrderCount() const
{
    return m_orders.size();
}

double OrdersService::getTotalRevenue() const
{
    double total = 0;
    for (const Order& order : m_orders) {
        total += order.quantity * order.price;
    }
    return total;
}

QString OrdersService::generateId() const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).left(8);
}

} // namespace orders
