#include "internal_service_space.h"
#include "global_environment.h"
#include "pobject_builder.h"

InternalServiceSpace::InternalServiceSpace(const QString& name)
    : AbstractServiceSpace{name} {
  Thread = std::unique_ptr<QThread>(new QThread());
  Thread->start();

  PObjectBuilder* eru = PObjectBuilder::instance();
  eru->setThread(Thread.get());

  Logger = std::unique_ptr<LogSystem>(eru->create<LogSystem>("LogSystem"));

  // Создаем глобальную среду для сигналов и слотов объектов
  GlobalEnvironment::instance()->moveToThread(Thread.get());
}

InternalServiceSpace::~InternalServiceSpace() {
  Thread->quit();
  Thread->wait();
}
