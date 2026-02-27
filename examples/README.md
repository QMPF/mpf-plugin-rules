# MPF Plugin Rules 样例代码

本目录展示 rules 插件如何作为"事件响应者"与其他插件协作。

## 文件说明

| 文件 | 展示内容 |
|------|----------|
| `01_subscribe_orders_events.cpp` | 订阅 orders 插件的事件并响应 |
| `02_lightweight_plugin.cpp` | 纯 SDK 依赖的轻量插件模式 |
| `03_cross_plugin_qml.qml` | QML 层的跨插件事件通信 |

## Rules 插件的角色

Rules 插件是一个典型的"事件响应者"：
- **不主动发起**业务流程
- **监听** orders 等插件的事件
- **响应**事件并执行规则检查
- **发布**检查结果供其他插件消费

```
跨插件通信流程：

orders 插件                     rules 插件
    │                               │
    │ publish("orders/created")     │
    │──────────────────────────────→│
    │                               │ 执行规则检查
    │                               │
    │ onEventPublished              │
    │←──────────────────────────────│
    │  ("rules/check/completed")    │
    │                               │
    │ 更新订单状态                    │
```

## 轻量插件模式

Rules 插件展示了一种"纯 SDK 依赖"的轻量模式：
- 只依赖 mpf-sdk（不需要 mpf-http-client 等额外库）
- 通过 EventBus 与其他插件通信（无需直接依赖）
- 最小化的 CMakeLists.txt 配置
