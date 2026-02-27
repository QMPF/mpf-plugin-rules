/**
 * =============================================================================
 * 样例 01: 订阅 orders 插件的事件
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * Rules 插件如何监听 orders 插件的事件，实现跨插件业务协作。
 * 这是 MPF 框架中跨插件通信的核心场景。
 *
 * 【关键点】
 * 1. rules 插件不需要在编译时依赖 orders 插件
 * 2. 两者只通过 EventBus 的 topic 字符串约定通信
 * 3. 即使 orders 插件不存在，rules 插件也能正常启动
 * 4. 这就是"松耦合"的实际体现
 *
 * 【通信契约】
 * 两个插件之间需要约定：
 * - Topic 名称：如 "orders/created"
 * - Data 格式：如 { orderId, customerName, totalAmount }
 * 这些约定应该记录在文档中，而不是在代码中强制。
 * =============================================================================
 */

#include <mpf/interfaces/iplugin.h>
#include <mpf/interfaces/ieventbus.h>
#include <mpf/service_registry.h>
#include <mpf/logger.h>

/**
 * 样例：RulesPlugin 的完整 EventBus 集成
 *
 * 展示如何在 start() 中订阅事件，
 * 在事件处理器中执行规则检查，
 * 并将结果发布回 EventBus。
 */

/*

class RulesPlugin : public QObject, public mpf::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MPF_IPlugin_iid FILE "../rules_plugin.json")
    Q_INTERFACES(mpf::IPlugin)

public:
    // ... 构造函数、析构函数 ...

    bool initialize(mpf::ServiceRegistry* registry) override
    {
        m_registry = registry;
        // Rules 插件不需要创建复杂的服务
        // 只需要一个简单的规则引擎
        m_ruleEngine = std::make_unique<RuleEngine>(this);
        return true;
    }

    bool start() override
    {
        MPF_LOG_INFO("RulesPlugin", "Starting event subscriptions...");
        
        m_eventBus = m_registry->get<mpf::IEventBus>();
        if (!m_eventBus) {
            MPF_LOG_WARNING("RulesPlugin", "EventBus not available, running in standalone mode");
            return true;  // 不是致命错误
        }
        
        // =====================================================================
        // 订阅 orders 模块的所有事件
        // =====================================================================
        //
        // 使用 "orders/**" 通配符，匹配：
        // - orders/created
        // - orders/updated
        // - orders/status_changed
        // - orders/deleted
        // - orders/items/added （多级也能匹配）

        m_eventBus->subscribe(
            "orders/**",              // 通配符模式
            "com.biiz.rules",         // 订阅者 ID
            nullptr,                  // 使用信号而非回调
            mpf::SubscriptionOptions{}
        );
        
        // 也可以订阅特定事件（更精确）
        m_eventBus->subscribe(
            "orders/created",         // 精确匹配
            "com.biiz.rules",
            nullptr,                  // 使用信号而非回调
            mpf::SubscriptionOptions{
                .async = true,
                .priority = 10,       // 高优先级：确保规则检查先于其他处理
                .receiveOwnEvents = false
            }
        );
        
        // =====================================================================
        // 连接 eventPublished 信号
        // =====================================================================
        QObject* busObj = dynamic_cast<QObject*>(m_eventBus);
        if (busObj) {
            connect(busObj, SIGNAL(eventPublished(QString,QVariantMap,QString)),
                    this,   SLOT(onEvent(QString,QVariantMap,QString)));
        }
        
        MPF_LOG_INFO("RulesPlugin", "Event subscriptions active");
        return true;
    }

    void stop() override
    {
        // 【重要】清理所有订阅
        if (m_eventBus) {
            m_eventBus->unsubscribeAll("com.biiz.rules");
            MPF_LOG_INFO("RulesPlugin", "Unsubscribed from all events");
        }
    }

private slots:
    // =========================================================================
    // 事件处理器
    // =========================================================================
    void onEvent(const QString& topic,
                 const QVariantMap& data,
                 const QString& senderId)
    {
        // 过滤：只处理 orders 相关事件
        if (!topic.startsWith("orders/")) return;
        
        MPF_LOG_DEBUG("RulesPlugin",
            QString("Received event: %1 from %2").arg(topic, senderId));
        
        // -----------------------------------------------------------------
        // 处理"订单创建"事件
        // -----------------------------------------------------------------
        if (topic == "orders/created") {
            QString orderId = data.value("orderId").toString();
            double amount = data.value("totalAmount").toDouble();
            QString customer = data.value("customerName").toString();
            
            MPF_LOG_INFO("RulesPlugin",
                QString("Checking rules for new order %1 (amount: %2)")
                    .arg(orderId).arg(amount));
            
            // 执行规则检查
            bool passed = true;
            QString reason;
            
            // 规则1：金额上限检查
            if (amount > 10000) {
                passed = false;
                reason = "订单金额超过上限 ($10,000)";
            }
            
            // 规则2：客户名称检查
            if (customer.isEmpty()) {
                passed = false;
                reason = "客户名称为空";
            }
            
            // 发布规则检查结果
            if (m_eventBus) {
                m_eventBus->publish(
                    "rules/check/completed",     // 结果 topic
                    {
                        {"orderId", orderId},
                        {"passed",  passed},
                        {"reason",  reason},
                        {"checkedAt", QDateTime::currentMSecsSinceEpoch()}
                    },
                    "com.biiz.rules"             // 发送者
                );
                
                MPF_LOG_INFO("RulesPlugin",
                    QString("Rule check result for %1: %2")
                        .arg(orderId, passed ? "PASSED" : "FAILED: " + reason));
            }
        }
        // -----------------------------------------------------------------
        // 处理"订单状态变更"事件
        // -----------------------------------------------------------------
        else if (topic == "orders/status_changed") {
            QString orderId = data.value("orderId").toString();
            QString newStatus = data.value("newStatus").toString();
            
            // 某些状态变更需要额外的规则检查
            if (newStatus == "shipped") {
                MPF_LOG_INFO("RulesPlugin",
                    QString("Order %1 shipped, checking shipping rules...")
                        .arg(orderId));
                // ... 执行发货规则检查 ...
            }
        }
    }

private:
    mpf::ServiceRegistry* m_registry = nullptr;
    mpf::IEventBus* m_eventBus = nullptr;
    std::unique_ptr<RuleEngine> m_ruleEngine;
};

*/
