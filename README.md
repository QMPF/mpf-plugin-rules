# MPF Rules Plugin

Qt Modular Plugin Framework - 规则管理插件（示例）

## 概述

另一个示例业务插件，展示多插件共存场景。此插件复用了 orders 插件的数据模型（`OrdersService`, `OrderModel`），以不同的 QML 模块 URI（`Biiz.Rules`）注册，演示了：

- 多插件使用独立的 QML 模块 URI（`Biiz.Rules` vs `YourCo.Orders`）
- 复用已有 C++ 服务类作为不同业务模型
- 独立的菜单项和路由注册
- QML 文件别名映射（源文件名 → 输出别名）

## 依赖

- Qt 6.8+（Core, Gui, Qml, Quick）
- MPF foundation-sdk

> 注意：此插件不依赖 mpf-http-client，仅使用核心 SDK 接口。

## 构建

```bash
# 1. 安装 mpf-dev 和 SDK
mpf-dev setup

# 2. 初始化项目
mpf-dev init

# 3. 构建
cmake --preset dev
cmake --build build
```

## 开发调试

```bash
# 注册插件
mpf-dev link plugin rules ./build

# 运行
mpf-dev run
```

## 插件元数据

```json
{
    "id": "com.biiz.rules",
    "name": "Rules Plugin",
    "version": "1.0.0",
    "provides": ["IRulesService"],
    "qmlModules": ["Biiz.Rules"],
    "priority": 10
}
```

## 项目结构

```
mpf-plugin-rules/
├── CMakeLists.txt
├── rules_plugin.json         # 插件元数据
├── include/
│   ├── rules_plugin.h        # IPlugin 实现
│   ├── orders_service.h      # 复用的服务类
│   └── order_model.h         # 复用的数据模型
├── src/
│   ├── rules_plugin.cpp      # 插件生命周期
│   ├── orders_service.cpp    # 业务逻辑
│   └── order_model.cpp       # 列表模型
└── qml/
    ├── OrdersPage.qml        # → 别名 RulesPage.qml
    ├── OrderCard.qml          # → 别名 RuleCard.qml
    └── CreateOrderDialog.qml  # → 别名 CreateRuleDialog.qml
```

## 许可证

MIT License
