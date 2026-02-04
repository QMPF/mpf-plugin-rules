#pragma once

#include <QObject>
#include <mpf/interfaces/iplugin.h>

namespace rules {

class OrdersService;

/**
 * @brief Orders plugin implementation
 *
 * Demonstrates a complete business plugin with:
 * - Service registration
 * - QML module
 * - Route registration
 * - Settings usage
 */
class RulesPlugin : public QObject, public mpf::IPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MPF_IPlugin_iid FILE "../rules_plugin.json")
  Q_INTERFACES(mpf::IPlugin)

public:
  explicit RulesPlugin(QObject *parent = nullptr);
  ~RulesPlugin() override;

  // IPlugin interface
  bool initialize(mpf::ServiceRegistry *registry) override;
  bool start() override;
  void stop() override;
  QJsonObject metadata() const override;
  QString qmlModuleUri() const override { return "YourCo.Orders"; }
  QString entryQml() const override { return QString(); }

private:
  void registerRoutes();
  void registerQmlTypes();

  mpf::ServiceRegistry *m_registry = nullptr;
  std::unique_ptr<OrdersService> m_ordersService;
};

} // namespace rules
