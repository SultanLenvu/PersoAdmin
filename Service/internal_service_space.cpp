#include "internal_service_space.h"

#include "global_environment.h"
#include "named_object_factory.h"
#include "pobject_builder.h"

InternalServiceSpace::InternalServiceSpace(const QString& name)
    : AbstractServiceSpace{name} {
  Thread = std::unique_ptr<QThread>(new QThread());
  Thread->start();

  //  PObjectBuilder* builder = PObjectBuilder::instance();
  //  builder->setThread(Thread.get());

  NamedObjectFactory factory(Thread.get());

  Logger = std::unique_ptr<LogSystem>(factory.create<LogSystem>("LogSystem"));

  // Создаем глобальную среду для сигналов и слотов объектов
  GlobalEnvironment::instance()->moveToThread(Thread.get());
}

InternalServiceSpace::~InternalServiceSpace() {
  Thread->quit();
  Thread->wait();
}
