/**
 * =============================================================================
 * 样例 02: 纯 SDK 依赖的轻量插件模式
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * 一个最小化的 MPF 插件是什么样的。
 * Rules 插件只依赖 mpf-sdk，不需要 mpf-http-client 等额外库。
 *
 * 【轻量插件 vs 完整插件 的对比】
 *
 * ┌─────────────────────┬──────────────────────┬──────────────────────┐
 * │                     │ 轻量插件 (rules)     │ 完整插件 (orders)    │
 * ├─────────────────────┼──────────────────────┼──────────────────────┤
 * │ 编译时依赖          │ mpf-sdk 只有          │ mpf-sdk + http-client │
 * │ 运行时依赖          │ ui-components, Theme  │ 同左                 │
 * │ 服务注册            │ 不注册               │ 注册 OrdersService    │
 * │ HTTP 请求           │ 不需要               │ 需要                 │
 * │ 跨插件通信          │ 只订阅事件           │ 发布 + 订阅          │
 * │ CMakeLists.txt      │ 最简配置             │ 包含 link 指令       │
 * └─────────────────────┴──────────────────────┴──────────────────────┘
 *
 * 【CMakeLists.txt 对比】
 *
 * 轻量插件：
 *   target_link_libraries(rules-plugin PRIVATE
 *       Qt6::Core Qt6::Quick
 *       MPF::foundation-sdk       # 只需要 SDK 接口
 *   )
 *
 * 完整插件：
 *   target_link_libraries(orders-plugin PRIVATE
 *       Qt6::Core Qt6::Quick Qt6::Network
 *       MPF::foundation-sdk       # SDK 接口
 *       MPF::mpf-http-client      # HTTP 客户端库
 *   )
 * =============================================================================
 */

/**
 * 样例：最小化的插件实现
 *
 * 这个插件只做三件事：
 * 1. 注册一个菜单项
 * 2. 注册一个 QML 页面
 * 3. 监听 EventBus 事件
 */

/*
class MinimalPlugin : public QObject, public mpf::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MPF_IPlugin_iid FILE "../minimal_plugin.json")
    Q_INTERFACES(mpf::IPlugin)

public:
    explicit MinimalPlugin(QObject* parent = nullptr) : QObject(parent) {}
    ~MinimalPlugin() override = default;

    // =========================================================================
    // initialize: 几乎什么都不做
    // =========================================================================
    bool initialize(mpf::ServiceRegistry* registry) override
    {
        m_registry = registry;
        
        // 轻量插件不需要创建服务实例
        // 也不需要注册 QML 类型（如果 QML 页面不使用 C++ 模型）
        
        return true;
    }

    // =========================================================================
    // start: 注册路由和菜单
    // =========================================================================
    bool start() override
    {
        // 注册路由
        auto* nav = m_registry->get<mpf::INavigation>();
        if (nav) {
            nav->registerRoute("minimal", findPageUrl());
        }
        
        // 注册菜单
        auto* menu = m_registry->get<mpf::IMenu>();
        if (menu) {
            mpf::MenuItem item;
            item.id       = "minimal";
            item.label    = "Minimal";
            item.icon     = "📋";
            item.route    = "minimal";
            item.pluginId = "com.example.minimal";
            item.order    = 50;
            menu->registerItem(item);
        }
        
        // 订阅事件（可选）
        auto* eventBus = m_registry->get<mpf::IEventBus>();
        if (eventBus) {
            eventBus->subscribe("orders/**", "com.example.minimal",
                                nullptr, mpf::SubscriptionOptions{});
            // 连接信号...
        }
        
        return true;
    }

    // =========================================================================
    // stop: 清理订阅
    // =========================================================================
    void stop() override
    {
        auto* eventBus = m_registry->get<mpf::IEventBus>();
        if (eventBus) {
            eventBus->unsubscribeAll("com.example.minimal");
        }
    }

    QJsonObject metadata() const override
    {
        return QJsonDocument::fromJson(R"({
            "id": "com.example.minimal",
            "name": "Minimal Plugin",
            "version": "1.0.0",
            "vendor": "Example",
            "priority": 50
        })").object();
    }

    QString qmlModuleUri() const override { return "Example.Minimal"; }

private:
    mpf::ServiceRegistry* m_registry = nullptr;
};
*/

/**
 * 样例：轻量插件的 QML 页面
 *
 * 轻量插件的 QML 页面只使用 Host 提供的全局对象和公共组件，
 * 不需要自己的 C++ 服务。
 */

// ---- MinimalPage.qml ----
//
// import QtQuick
// import QtQuick.Controls
// import QtQuick.Layouts
// import MPF.Components 1.0
//
// Page {
//     title: "Minimal Plugin"
//
//     background: Rectangle {
//         color: Theme ? Theme.backgroundColor : "#FAFAFA"
//     }
//
//     // 【无自有服务】
//     // 这个页面不 import 任何插件自己的 QML 模块
//     // 只使用 MPF.Components 和全局 Theme/EventBus
//
//     ColumnLayout {
//         anchors.fill: parent
//         anchors.margins: Theme ? Theme.spacingMedium : 16
//
//         MPFCard {
//             title: "轻量插件"
//             subtitle: "只依赖 mpf-sdk，无额外库"
//             Layout.fillWidth: true
//
//             Label {
//                 text: "这个插件展示了最小化的 MPF 插件模式。"
//                 color: Theme ? Theme.textColor : "#212121"
//                 wrapMode: Text.Wrap
//             }
//         }
//
//         // 【EventBus 监听】
//         // 即使是轻量插件也可以监听事件
//         MPFCard {
//             title: "事件监听"
//             Layout.fillWidth: true
//             Layout.fillHeight: true
//
//             property var events: []
//
//             Connections {
//                 target: EventBus
//                 function onEventPublished(topic, data, senderId) {
//                     // 记录所有事件
//                     events.push({topic: topic, time: new Date()})
//                     eventCount.text = "已收到 " + events.length + " 个事件"
//                 }
//             }
//
//             Label {
//                 id: eventCount
//                 text: "等待事件..."
//                 color: Theme ? Theme.textSecondaryColor : "#757575"
//             }
//         }
//     }
// }
