#include "internal_service_space.h"

#include "global_environment.h"
#include "named_object_factory.h"

InternalServiceSpace::InternalServiceSpace() {
  Thread = std::unique_ptr<QThread>(new QThread());
  Thread->start();

  NamedObjectFactory factory(Thread.get());
  Logger = std::unique_ptr<LogSystem>(factory.create<LogSystem>("LogSystem"));

  // Переносим глобальную среду для сигналов и слотов объектов
  GlobalEnvironment::instance()->moveToThread(Thread.get());
}

InternalServiceSpace::~InternalServiceSpace() {
  Thread->quit();
  Thread->wait();
}
