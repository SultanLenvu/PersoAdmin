#include "qsettings_configurator.h"

#include "global_environment.h"
#include "gui_kernel.h"

QSettingsConfigurator::QSettingsConfigurator(ConfigurableEntity* entity)
    : Entity(entity) {
  connectDependencies();
}

void QSettingsConfigurator::apply() {
  if (!Entity) {
    return;
  }

  Entity->loadSettings();
}

void QSettingsConfigurator::connectDependencies() {
  GuiKernel* gk = static_cast<GuiKernel*>(
      GlobalEnvironment::instance()->getObject("GuiKernel"));

  connect(gk, &GuiKernel::applySettings_signal, this,
          &QSettingsConfigurator::apply);
}
