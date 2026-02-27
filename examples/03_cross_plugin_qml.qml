/**
 * =============================================================================
 * 样例 03: QML 层的跨插件事件通信
 * =============================================================================
 *
 * 【这个样例展示了什么】
 * Rules 插件的 QML 页面如何：
 * 1. 监听 orders 插件发布的事件（如"订单创建"）
 * 2. 执行规则检查后发布结果事件
 * 3. 使用公共 UI 组件（MPFCard, MPFButton 等）
 * 4. 使用 Theme 保持视觉一致性
 *
 * 【运行时依赖】
 * - EventBus: Host 通过 context property 注入
 * - Theme: Host 通过 context property 注入
 * - MPF.Components: Host 设置 QML import path 后可用
 *
 * 这三个依赖都是运行时的，不需要编译时链接。
 * =============================================================================
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MPF.Components 1.0   // 运行时依赖：公共 UI 组件

Page {
    id: root
    title: qsTr("Rules - 跨插件通信示例")

    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FAFAFA"
    }

    // =========================================================================
    // 规则检查结果数据
    // =========================================================================
    ListModel {
        id: checkResultsModel
    }

    // =========================================================================
    // 【核心】EventBus 事件监听与响应
    // =========================================================================
    Connections {
        target: EventBus

        function onEventPublished(topic, data, senderId) {
            // -----------------------------------------------------------------
            // 监听 orders 插件的"订单创建"事件
            // -----------------------------------------------------------------
            if (topic === "orders/created") {
                console.log("Rules: 收到订单创建事件, orderId:", data.orderId)

                // 执行规则检查（QML 层的简单逻辑）
                var passed = true
                var reason = ""

                // 规则1: 金额检查
                if (data.totalAmount > 10000) {
                    passed = false
                    reason = "金额超过 $10,000 上限"
                }

                // 规则2: 客户名检查
                if (!data.customerName || data.customerName.length === 0) {
                    passed = false
                    reason = "客户名称为空"
                }

                // 记录检查结果
                checkResultsModel.insert(0, {
                    orderId: data.orderId,
                    customerName: data.customerName || "未知",
                    amount: data.totalAmount || 0,
                    passed: passed,
                    reason: reason,
                    time: new Date().toLocaleTimeString()
                })

                // ---------------------------------------------------------
                // 【关键】发布规则检查结果事件
                // orders 插件可以监听这个事件来更新订单状态
                // ---------------------------------------------------------
                EventBus.publish(
                    "rules/check/completed",
                    {
                        "orderId":   data.orderId,
                        "passed":    passed,
                        "reason":    reason,
                        "ruleCount": 2,
                        "checkedBy": "com.biiz.rules"
                    },
                    "com.biiz.rules"
                )
            }

            // -----------------------------------------------------------------
            // 监听订单状态变更事件
            // -----------------------------------------------------------------
            if (topic === "orders/status_changed") {
                console.log("Rules: 订单", data.orderId,
                           "状态从", data.oldStatus, "变为", data.newStatus)

                // 某些状态变更可能触发额外规则
                if (data.newStatus === "cancelled") {
                    // 记录取消事件
                    checkResultsModel.insert(0, {
                        orderId: data.orderId,
                        customerName: "状态变更",
                        amount: 0,
                        passed: true,
                        reason: "订单已取消，无需检查",
                        time: new Date().toLocaleTimeString()
                    })
                }
            }
        }
    }

    // =========================================================================
    // 初始化订阅
    // =========================================================================
    Component.onCompleted: {
        // 注册订阅（告诉 EventBus "我对这些主题感兴趣"）
        EventBus.subscribeSimple("orders/**", "com.biiz.rules.qml")
        console.log("Rules QML: 已订阅 orders/**")
    }

    Component.onDestruction: {
        EventBus.unsubscribeAll("com.biiz.rules.qml")
    }

    // =========================================================================
    // UI 布局
    // =========================================================================
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme ? Theme.spacingMedium : 16
        spacing: Theme ? Theme.spacingMedium : 16

        // 标题栏
        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("规则检查结果")
                font.pixelSize: 20
                font.bold: true
                color: Theme ? Theme.textColor : "#212121"
                Layout.fillWidth: true
            }

            // 统计信息
            Label {
                text: qsTr("共 %1 条记录").arg(checkResultsModel.count)
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
        }

        // 统计卡片
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme ? Theme.spacingMedium : 16

            MPFCard {
                Layout.fillWidth: true
                implicitHeight: 70

                ColumnLayout {
                    anchors.centerIn: parent
                    Label {
                        text: "✓ 通过"
                        color: Theme ? Theme.successColor : "green"
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Label {
                        text: {
                            var count = 0
                            for (var i = 0; i < checkResultsModel.count; i++) {
                                if (checkResultsModel.get(i).passed) count++
                            }
                            return count.toString()
                        }
                        font.pixelSize: 24
                        font.bold: true
                        color: Theme ? Theme.successColor : "green"
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            MPFCard {
                Layout.fillWidth: true
                implicitHeight: 70

                ColumnLayout {
                    anchors.centerIn: parent
                    Label {
                        text: "✗ 失败"
                        color: Theme ? Theme.errorColor : "red"
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Label {
                        text: {
                            var count = 0
                            for (var i = 0; i < checkResultsModel.count; i++) {
                                if (!checkResultsModel.get(i).passed) count++
                            }
                            return count.toString()
                        }
                        font.pixelSize: 24
                        font.bold: true
                        color: Theme ? Theme.errorColor : "red"
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }
        }

        // 检查结果列表
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: Theme ? Theme.spacingSmall : 8
            model: checkResultsModel

            delegate: MPFCard {
                width: ListView.view.width
                implicitHeight: delegateContent.implicitHeight + 32

                ColumnLayout {
                    id: delegateContent
                    anchors {
                        left: parent.left; right: parent.right
                        verticalCenter: parent.verticalCenter
                        margins: Theme ? Theme.spacingMedium : 16
                    }
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true

                        // 状态图标
                        Label {
                            text: model.passed ? "✓" : "✗"
                            font.pixelSize: 16
                            font.bold: true
                            color: model.passed
                                   ? (Theme ? Theme.successColor : "green")
                                   : (Theme ? Theme.errorColor : "red")
                        }

                        // 订单 ID
                        Label {
                            text: "订单 #" + model.orderId
                            font.bold: true
                            color: Theme ? Theme.textColor : "#212121"
                        }

                        // 客户名
                        Label {
                            text: model.customerName
                            color: Theme ? Theme.textSecondaryColor : "#757575"
                        }

                        Item { Layout.fillWidth: true }

                        // 时间
                        Label {
                            text: model.time
                            font.pixelSize: 11
                            color: Theme ? Theme.textSecondaryColor : "#999"
                        }
                    }

                    // 失败原因
                    Label {
                        visible: !model.passed
                        text: "原因: " + model.reason
                        color: Theme ? Theme.errorColor : "red"
                        font.pixelSize: 12
                    }
                }
            }

            // 空状态
            Label {
                anchors.centerIn: parent
                visible: checkResultsModel.count === 0
                text: qsTr("等待订单事件...\n在 orders 页面创建一个订单试试")
                color: Theme ? Theme.textSecondaryColor : "#757575"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
