#include "rules_plugin.h"
#include "orders_service.h"
#include "order_model.h"

#include <mpf/service_registry.h>
#include <mpf/interfaces/inavigation.h>
#include <mpf/interfaces/imenu.h>
#include <mpf/logger.h>

#include <QJsonDocument>
#include <QQmlEngine>

namespace rules {

RulesPlugin::RulesPlugin(QObject* parent)
    : QObject(parent)
{
}

RulesPlugin::~RulesPlugin() = default;

bool RulesPlugin::initialize(mpf::ServiceRegistry* registry)
{
    m_registry = registry;
    
    MPF_LOG_INFO("OrdersPlugin", "Initializing...");
    
    // Create and register our service
    m_ordersService = std::make_unique<OrdersService>(this);
    
    // Register QML types
    registerQmlTypes();
    
    MPF_LOG_INFO("OrdersPlugin", "Initialized successfully");
    return true;
}

bool RulesPlugin::start()
{
    MPF_LOG_INFO("OrdersPlugin", "Starting...");
    
    // Register routes with navigation service
    registerRoutes();
    
    // Add some sample data for demo
    m_ordersService->createOrder({
        {"customerName", "John Doe"},
        {"productName", "Widget Pro"},
        {"quantity", 2},
        {"price", 99.99},
        {"status", "pending"}
    });
    
    m_ordersService->createOrder({
        {"customerName", "Jane Smith"},
        {"productName", "Gadget X"},
        {"quantity", 1},
        {"price", 149.99},
        {"status", "processing"}
    });
    
    m_ordersService->createOrder({
        {"customerName", "Bob Wilson"},
        {"productName", "Tool Kit"},
        {"quantity", 3},
        {"price", 49.99},
        {"status", "shipped"}
    });
    
    MPF_LOG_INFO("OrdersPlugin", "Started with sample orders");
    return true;
}

void RulesPlugin::stop()
{
    MPF_LOG_INFO("OrdersPlugin", "Stopping...");
}

QJsonObject RulesPlugin::metadata() const
{
    // This should match orders_plugin.json
    return QJsonDocument::fromJson(R"({
        "id": "com.yourco.orders",
        "name": "Orders Plugin",
        "version": "1.0.0",
        "description": "Order management functionality",
        "vendor": "YourCo",
        "requires": [
            {"type": "service", "id": "INavigation", "min": "1.0"}
        ],
        "provides": ["OrdersService"],
        "qmlModules": ["YourCo.Orders"],
        "priority": 10
    })").object();
}

void RulesPlugin::registerRoutes()
{
    auto* nav = m_registry->get<mpf::INavigation>();
    if (nav) {
        nav->registerRoute("orders", "qrc:/YourCo/Orders/qml/OrdersPage.qml");
        nav->registerRoute("orders/detail", "qrc:/YourCo/Orders/qml/OrderDetailPage.qml");
        MPF_LOG_DEBUG("OrdersPlugin", "Registered navigation routes");
    }
    
    // Register menu item
    auto* menu = m_registry->get<mpf::IMenu>();
    if (menu) {
        mpf::MenuItem item;
        item.id = "orders";
        item.label = tr("Orders");
        item.icon = "📦";
        item.route = "orders";
        item.pluginId = "com.yourco.orders";
        item.order = 10;
        item.group = "Business";
        
        bool registered = menu->registerItem(item);
        if (!registered) {
            MPF_LOG_WARNING("OrdersPlugin", "Failed to register menu item");
            return;
        }
        
        // Update badge with order count
        menu->setBadge("orders", QString::number(m_ordersService->getOrderCount()));
        
        // Connect to update badge when orders change
        connect(m_ordersService.get(), &OrdersService::ordersChanged, this, [this, menu]() {
            menu->setBadge("orders", QString::number(m_ordersService->getOrderCount()));
        });
        
        MPF_LOG_DEBUG("OrdersPlugin", "Registered menu item");
    } else {
        MPF_LOG_WARNING("OrdersPlugin", "Menu service not available");
    }
}

void RulesPlugin::registerQmlTypes()
{
    // Register OrdersService as singleton
    qmlRegisterSingletonInstance("YourCo.Orders", 1, 0, "OrdersService", m_ordersService.get());
    
    // Register OrderModel
    qmlRegisterType<OrderModel>("YourCo.Orders", 1, 0, "OrderModel");
    
    MPF_LOG_DEBUG("OrdersPlugin", "Registered QML types");
}

} // namespace rules
