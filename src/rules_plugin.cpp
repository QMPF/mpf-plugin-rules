#include "rules_plugin.h"
#include "orders_service.h"
#include "order_model.h"

#include <mpf/service_registry.h>
#include <mpf/interfaces/inavigation.h>
#include <mpf/interfaces/imenu.h>
#include <mpf/logger.h>

#include <QJsonDocument>
#include <QQmlEngine>
#include <QFile>

namespace rules {

RulesPlugin::RulesPlugin(QObject* parent)
    : QObject(parent)
{
}

RulesPlugin::~RulesPlugin() = default;

bool RulesPlugin::initialize(mpf::ServiceRegistry* registry)
{
    m_registry = registry;
    
    MPF_LOG_INFO("RulesPlugin", "Initializing...");
    
    // 调试：检查 qrc 资源是否可访问
    QStringList resourcesToCheck = {
        ":/Biiz/Rules/qml/OrdersPage.qml",
        "qrc:/Biiz/Rules/qml/OrdersPage.qml"
    };
    for (const QString& res : resourcesToCheck) {
        QFile f(res);
        MPF_LOG_DEBUG("RulesPlugin", 
            QString("Resource check: %1 exists=%2").arg(res).arg(f.exists() ? "YES" : "NO").toStdString().c_str());
    }
    
    // Create and register our service
    m_ordersService = std::make_unique<orders::OrdersService>(this);
    
    // Register QML types
    registerQmlTypes();
    
    MPF_LOG_INFO("RulesPlugin", "Initialized successfully");
    return true;
}

bool RulesPlugin::start()
{
    MPF_LOG_INFO("RulesPlugin", "Starting...");
    
    // Register routes with navigation service
    registerRoutes();
    
    // Add some sample data for demo
    m_ordersService->createOrder({
        {"customerName", "Rule A"},
        {"productName", "Validation Rule"},
        {"quantity", 1},
        {"price", 0},
        {"status", "active"}
    });
    
    m_ordersService->createOrder({
        {"customerName", "Rule B"},
        {"productName", "Approval Rule"},
        {"quantity", 1},
        {"price", 0},
        {"status", "active"}
    });
    
    MPF_LOG_INFO("RulesPlugin", "Started with sample rules");
    return true;
}

void RulesPlugin::stop()
{
    MPF_LOG_INFO("RulesPlugin", "Stopping...");
}

QJsonObject RulesPlugin::metadata() const
{
    return QJsonDocument::fromJson(R"({
        "id": "com.biiz.rules",
        "name": "Rules Plugin",
        "version": "1.0.0",
        "description": "Business rules management",
        "vendor": "Biiz",
        "requires": [
            {"type": "service", "id": "INavigation", "min": "1.0"}
        ],
        "provides": ["RulesService"],
        "qmlModules": ["Biiz.Rules"],
        "priority": 20
    })").object();
}

void RulesPlugin::registerRoutes()
{
    auto* nav = m_registry->get<mpf::INavigation>();
    if (nav) {
        // 使用 qrc:// 路径 - QML 资源已通过 qt_add_qml_module 编译到插件中
        // CMakeLists.txt 设置了 RESOURCE_PREFIX /，所以路径是 :/URI/QML_FILES路径
        nav->registerRoute("rules", "qrc:/Biiz/Rules/qml/OrdersPage.qml");
        nav->registerRoute("rules/detail", "qrc:/Biiz/Rules/qml/OrderDetailPage.qml");
        MPF_LOG_DEBUG("RulesPlugin", "Registered navigation routes (qrc:/Biiz/Rules/qml/)");
    }
    
    // Register menu item
    auto* menu = m_registry->get<mpf::IMenu>();
    if (menu) {
        mpf::MenuItem item;
        item.id = "rules";              // Changed from "orders"
        item.label = tr("Rules");       // Changed from "Orders"
        item.icon = "📋";               // Changed icon
        item.route = "rules";           // Changed from "orders"
        item.pluginId = "com.biiz.rules";
        item.order = 20;
        item.group = "Business";
        
        bool registered = menu->registerItem(item);
        if (!registered) {
            MPF_LOG_WARNING("RulesPlugin", "Failed to register menu item");
            return;
        }
        
        // Update badge with rule count
        menu->setBadge("rules", QString::number(m_ordersService->getOrderCount()));
        
        // Connect to update badge when rules change
        connect(m_ordersService.get(), &orders::OrdersService::ordersChanged, this, [this, menu]() {
            menu->setBadge("rules", QString::number(m_ordersService->getOrderCount()));
        });
        
        MPF_LOG_DEBUG("RulesPlugin", "Registered menu item");
    } else {
        MPF_LOG_WARNING("RulesPlugin", "Menu service not available");
    }
}

void RulesPlugin::registerQmlTypes()
{
    // Register service as singleton (using Biiz.Rules URI)
    qmlRegisterSingletonInstance("Biiz.Rules", 1, 0, "RulesService", m_ordersService.get());
    
    // Register model
    qmlRegisterType<orders::OrderModel>("Biiz.Rules", 1, 0, "RuleModel");
    
    MPF_LOG_DEBUG("RulesPlugin", "Registered QML types");
}

} // namespace rules
