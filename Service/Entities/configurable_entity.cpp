#include "configurable_entity.h"

#include "global_environment.h"
#include "gui_kernel.h"

ConfigurableEntity::ConfigurableEntity()
{
  Configurator = new QSettingsConfigurator(this);
}

~ConfigurableEntity() {}

ConfigurableEntity::QSettingsConfigurator::QSettingsConfigurator(
    ConfigurableEntity* entity) {}

ConfigurableEntity::QSettingsConfigurator::~QSettingsConfigurator() {}

void ConfigurableEntity::QSettingsConfigurator::apply() {}

void ConfigurableEntity::QSettingsConfigurator::connectDependencies() {
  GuiKernel* gk = static_cast<GuiKernel*>(
      GlobalEnvironment::instance()->getObject("GuiKernel"));

  connect(gk, &GuiKernel::applySettings_signal, this,
          &QSettingsConfigurator::apply);
}
