#include "rules_plugin.h"
#include "rules_service.h"
#include "rule_model.h"
#include "demo_service.h"

#include <mpf/service_registry.h>
#include <mpf/interfaces/inavigation.h>
#include <mpf/interfaces/imenu.h>
#include <mpf/interfaces/ieventbus.h>
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
        ":/Biiz/Rules/RulesPage.qml",
        "qrc:/Biiz/Rules/RulesPage.qml"
    };
    for (const QString& res : resourcesToCheck) {
        QFile f(res);
        MPF_LOG_DEBUG("RulesPlugin",
            QString("Resource check: %1 exists=%2").arg(res).arg(f.exists() ? "YES" : "NO").toStdString().c_str());
    }
    
    // Create and register our service
    m_rulesService = std::make_unique<RulesService>(this);

    // Demo service for framework showcase
    m_demoService = std::make_unique<DemoService>("com.biiz.rules", this);

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

    // Connect DemoService to EventBus for cross-plugin messaging
    auto* eventBus = m_registry->get<mpf::IEventBus>();
    if (eventBus) {
        auto* eventBusObj = dynamic_cast<QObject*>(eventBus);
        if (eventBusObj) {
            m_demoService->connectToEventBus(eventBusObj, "demo/rules/");
        }
    }

    // Add some sample data for demo
    m_rulesService->createRule({
        {"customerName", "Rule A"},
        {"productName", "Validation Rule"},
        {"quantity", 1},
        {"price", 0},
        {"status", "active"}
    });
    
    m_rulesService->createRule({
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
        // QML 文件统一从 qrc 资源加载（由 qt_add_qml_module 嵌入 DLL）
        nav->registerRoute("rules", "qrc:/Biiz/Rules/RulesPage.qml");
        MPF_LOG_INFO("RulesPlugin", "Registered route: rules (qrc)");

        nav->registerRoute("rules-demo", "qrc:/Biiz/Rules/DemoPage.qml");
        MPF_LOG_INFO("RulesPlugin", "Registered route: rules-demo (qrc)");
    }
    
    // Register menu item
    auto* menu = m_registry->get<mpf::IMenu>();
    if (menu) {
        mpf::MenuItem item;
        item.id = "rules";
        item.label = tr("Rules");
        item.icon = "📋";
        item.route = "rules";
        item.pluginId = "com.biiz.rules";
        item.order = 20;
        item.group = "Business";
        
        bool registered = menu->registerItem(item);
        if (!registered) {
            MPF_LOG_WARNING("RulesPlugin", "Failed to register menu item");
            return;
        }
        
        // Update badge with rule count
        menu->setBadge("rules", QString::number(m_rulesService->getRuleCount()));
        
        // Connect to update badge when rules change
        connect(m_rulesService.get(), &RulesService::rulesChanged, this, [this, menu]() {
            menu->setBadge("rules", QString::number(m_rulesService->getRuleCount()));
        });
        
        MPF_LOG_DEBUG("RulesPlugin", "Registered menu item");

        // Register demo menu item
        mpf::MenuItem demoItem;
        demoItem.id = "rules-demo";
        demoItem.label = tr("Rules Demo");
        demoItem.icon = "\xF0\x9F\x8E\xA8";  // 🎨
        demoItem.route = "rules-demo";
        demoItem.pluginId = "com.biiz.rules";
        demoItem.order = 25;
        demoItem.group = "Demo";
        menu->registerItem(demoItem);
    } else {
        MPF_LOG_WARNING("RulesPlugin", "Menu service not available");
    }
}

void RulesPlugin::registerQmlTypes()
{
    // Register service as singleton (using Biiz.Rules URI)
    qmlRegisterSingletonInstance("Biiz.Rules", 1, 0, "RulesService", m_rulesService.get());
    
    // Register model
    qmlRegisterType<RuleModel>("Biiz.Rules", 1, 0, "RuleModel");

    // Register DemoService singleton for QML
    qmlRegisterSingletonInstance("Biiz.Rules", 1, 0, "DemoService", m_demoService.get());

    MPF_LOG_DEBUG("RulesPlugin", "Registered QML types");
}

} // namespace rules
