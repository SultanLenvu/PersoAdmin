#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include <memory>

#include "settings_apply_connector.h"

class ConfigurableObject1 {
  friend class SettingsApplyBundle;

 private:
  std::unique_ptr<SettingsApplyBundle> Connector;

 public:
  ConfigurableObject1();
  virtual ~ConfigurableObject1();

 private:
  virtual void loadSettings(void) = 0;
};

#endif  // CONFIGURABLEOBJECT_H
