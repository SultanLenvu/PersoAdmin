#include "configurable_object.h"

#include "global_environment.h"
#include "gui_kernel.h"

ConfigurableObject::ConfigurableObject() {
  Connector =
      std::unique_ptr<SettingsApplyBundle>(new SettingsApplyBundle(this));
}

SettingsApplyBundle::SettingsApplyBundle(ConfigurableObject* object) {
  Object = object;

  GuiKernel* gk = static_cast<GuiKernel*>(
      GlobalEnvironment::instance()->getObject("GuiKernel"));

  connect(gk, &GuiKernel::applySettings_signal, this,
          &SettingsApplyBundle::apply);
}

SettingsApplyBundle::~SettingsApplyBundle() {}

void SettingsApplyBundle::apply() {
  Object->loadSettings();
}
