#include "service_object_space.h"

#include "global_environment.h"
#include "named_object_factory.h"

ServiceObjectSpace::ServiceObjectSpace() {
  Thread.start();

  NamedObjectFactory factory(&Thread);
  ConfigUpdater = std::unique_ptr<ConfigurationManager>(
      factory.create<ConfigurationManager>("ConfigurationManager"));
  Logger = std::unique_ptr<LogSystem>(factory.create<LogSystem>("LogSystem"));

  // Переносим глобальную среду для сигналов и слотов объектов
  GlobalEnvironment::instance()->moveToThread(&Thread);
}

ServiceObjectSpace::~ServiceObjectSpace() {
  Thread.quit();
  Thread.wait();
}
