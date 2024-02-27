#include "configurable_object.h"
#include "global_environment.h"
#include "gui_kernel.h"

ConfigurableObject::ConfigurableObject(const QString& name) : PObject{name} {
  connectDependencies();
}

ConfigurableObject::~ConfigurableObject() {}

void ConfigurableObject::applySettings() {
  sendLog("Применение новых настроек.");
  loadSettings();
}

void ConfigurableObject::connectDependencies() {}

void ConfigurableObject::loadSettings() {
  GuiKernel* gk = static_cast<GuiKernel*>(
      GlobalEnvironment::instance()->getObject("GuiKernel"));

  connect(gk, &GuiKernel::applySettings_signal, this,
          &ConfigurableObject::applySettings);
}
