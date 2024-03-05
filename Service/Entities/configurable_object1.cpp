#include "configurable_object1.h"

#include "global_environment.h"
#include "gui_kernel.h"

ConfigurableObject1::ConfigurableObject1() {
  //    Connector =
  //      std::unique_ptr<SettingsApplyBundle>(new
  //      SettingsApplyBundle(this));
}

ConfigurableObject1::~ConfigurableObject1() {}

// SettingsApplyBundle::SettingsApplyBundle(ConfigurableObject1* object) {
//   Object = object;

//  GuiKernel* gk = static_cast<GuiKernel*>(
//      GlobalEnvironment::instance()->getObject("GuiKernel"));

//  connect(gk, &GuiKernel::applySettings_signal, this,
//          &SettingsApplyBundle::apply);
//}

// SettingsApplyBundle::~SettingsApplyBundle() {}

// void SettingsApplyBundle::apply() {
//   Object->loadSettings();
// }
