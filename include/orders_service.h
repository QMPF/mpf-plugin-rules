#pragma once

#include <QObject>
#include <QList>
#include <QVariantMap>
#include <QDateTime>

namespace orders {

struct Order {
    QString id;
    QString customerName;
    QString productName;
    int quantity;
    double price;
    QString status;  // pending, processing, shipped, delivered, cancelled
    QDateTime createdAt;
    QDateTime updatedAt;
    
    QVariantMap toVariantMap() const;
    static Order fromVariantMap(const QVariantMap& map);
};

/**
 * @brief Orders business service
 * 
 * Provides order management functionality.
 * This could be exposed as an interface if other plugins need it.
 */
class OrdersService : public QObject
{
    Q_OBJECT

public:
    explicit OrdersService(QObject* parent = nullptr);
    ~OrdersService() override;

    // CRUD operations
    Q_INVOKABLE QVariantList getAllOrders() const;
    Q_INVOKABLE QVariantMap getOrder(const QString& id) const;
    Q_INVOKABLE QString createOrder(const QVariantMap& data);
    Q_INVOKABLE bool updateOrder(const QString& id, const QVariantMap& data);
    Q_INVOKABLE bool deleteOrder(const QString& id);
    
    // Business operations
    Q_INVOKABLE bool updateStatus(const QString& id, const QString& status);
    Q_INVOKABLE QVariantList getOrdersByStatus(const QString& status) const;
    Q_INVOKABLE int getOrderCount() const;
    Q_INVOKABLE double getTotalRevenue() const;

signals:
    void orderCreated(const QString& id);
    void orderUpdated(const QString& id);
    void orderDeleted(const QString& id);
    void ordersChanged();

private:
    QString generateId() const;
    
    QList<Order> m_orders;
};

} // namespace orders
