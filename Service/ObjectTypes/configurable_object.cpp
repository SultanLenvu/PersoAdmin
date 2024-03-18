#include "configurable_object.h"

#include "configuration_manager.h"
#include "global_environment.h"

ConfigurableObject::ConfigurableObject() {
  Connector = std::unique_ptr<ConfigurationSystemConnection>(
      new ConfigurationSystemConnection(this));
}

ConfigurationSystemConnection::ConfigurationSystemConnection(
    ConfigurableObject* object) {
  Object = object;

  ConfigurationSystem* cum = static_cast<ConfigurationSystem*>(
      GlobalEnvironment::instance()->getObject("ConfigurationSystem"));

  connect(cum, &ConfigurationSystem::applySettings_signal, this,
          &ConfigurationSystemConnection::apply);
}

void ConfigurationSystemConnection::apply() {
  Object->loadSettings();
}
