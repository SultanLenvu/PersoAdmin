#include "configurable_object1.h"

ConfigurableObject1::ConfigurableObject1() {
  Connector =
      std::unique_ptr<SettingsApplyBundle>(new SettingsApplyBundle(this));
}

ConfigurableObject1::~ConfigurableObject1() {}
