#include "configurable_object_new.h"

#include "global_environment.h"
#include "gui_kernel.h"

ConfigurableObjectNew::ConfigurableObjectNew() {
  Connector =
      std::unique_ptr<SettingsApplyBundle>(new SettingsApplyBundle(this));
}

ConfigurableObjectNew::~ConfigurableObjectNew() {}

SettingsApplyBundle::SettingsApplyBundle(ConfigurableObjectNew* object) {
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
