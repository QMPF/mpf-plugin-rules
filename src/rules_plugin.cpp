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
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>

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
        ":/Biiz/Rules/qml/RulesPage.qml",
        "qrc:/Biiz/Rules/qml/RulesPage.qml"
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
        // 构建 QML 搜索路径列表（优先级从高到低）
        QStringList searchPaths;
        QString appDir = QCoreApplication::applicationDirPath();
        
        // 1. MPF_SDK_ROOT 环境变量（mpf-dev 设置）
        QString sdkRoot = qEnvironmentVariable("MPF_SDK_ROOT");
        if (!sdkRoot.isEmpty()) {
            searchPaths << QDir::cleanPath(sdkRoot + "/qml");
        }
        
        // 2. QML_IMPORT_PATH 环境变量
        QString qmlImportPaths = qEnvironmentVariable("QML_IMPORT_PATH");
        searchPaths << qmlImportPaths.split(QDir::listSeparator(), Qt::SkipEmptyParts);
        
        // 3. 应用程序相对路径（标准 SDK 安装布局）
        searchPaths << QDir::cleanPath(appDir + "/../qml");
        
        // 4. 应用程序同级 qml 目录（开发模式）
        searchPaths << QDir::cleanPath(appDir + "/qml");
        
        // 查找 QML 模块目录
        QString qmlFile;
        for (const QString& basePath : searchPaths) {
            QString candidate = QDir::cleanPath(basePath + "/Biiz/Rules/RulesPage.qml");
            if (QFile::exists(candidate)) {
                qmlFile = candidate;
                break;
            }
        }
        
        if (qmlFile.isEmpty()) {
            MPF_LOG_ERROR("RulesPlugin", "Could not find Biiz/Rules/RulesPage.qml!");
            MPF_LOG_ERROR("RulesPlugin", QString("Searched paths: %1").arg(searchPaths.join("; ")).toStdString().c_str());
            return;
        }
        
        QString rulesPage = QUrl::fromLocalFile(qmlFile).toString();
        
        MPF_LOG_INFO("RulesPlugin", QString("Rules page URL: %1").arg(rulesPage).toStdString().c_str());
        
        // 注册主页面（内部导航使用 Popup）
        nav->registerRoute("rules", rulesPage);

        // Register demo page route
        QString qmlBase = QFileInfo(qmlFile).absolutePath();
        QString demoFile = QDir::cleanPath(qmlBase + "/DemoPage.qml");
        if (QFile::exists(demoFile)) {
            QString demoPage = QUrl::fromLocalFile(demoFile).toString();
            nav->registerRoute("rules-demo", demoPage);
            MPF_LOG_INFO("RulesPlugin", "Registered route: rules-demo");
        }

        MPF_LOG_INFO("RulesPlugin", "Registered route: rules");
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
