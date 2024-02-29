#include "internal_service_space.h"
#include "global_environment.h"
#include "pobject_builder.h"

InternalServiceSpace::InternalServiceSpace(const QString& name)
    : AbstractServiceSpace{name} {
  Thread = std::unique_ptr<QThread>(new QThread());
  Thread->start();

  PObjectBuilder builder(Thread.get());

  Logger = std::unique_ptr<LogSystem>(builder.build<LogSystem>("LogSystem"));

  //  connect(Thread.get(), &QThread::started, Logger.get(),
  //          &LogSystem::onInstanceThreadStarted, Qt::DirectConnection);

  GlobalEnvironment::instance()->moveToThread(Thread.get());
  //  Logger->moveToThread(Thread.get());

  // Создаем глобальную среду для сигналов и слотов объектов
}

InternalServiceSpace::~InternalServiceSpace() {
  Thread->quit();
  Thread->wait();
}

void InternalServiceSpace::init() {}
