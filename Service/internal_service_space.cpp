#include "internal_service_space.h"
#include "global_environment.h"

InternalServiceSpace::InternalServiceSpace(const QString& name)
    : AbstractServiceSpace{name} {
  Thread = std::unique_ptr<QThread>(new QThread());
  Logger = std::unique_ptr<LogSystem>(new LogSystem("LogSystem"));

  connect(Thread.get(), &QThread::started, Logger.get(),
          &LogSystem::onInstanceThreadStarted, Qt::DirectConnection);

  GlobalEnvironment::instance()->moveToThread(Thread.get());
  Logger->moveToThread(Thread.get());

  // Создаем глобальную среду для сигналов и слотов объектов
  Thread->start();
}

InternalServiceSpace::~InternalServiceSpace() {
  Thread->quit();
  Thread->wait();
}

void InternalServiceSpace::init() {}
