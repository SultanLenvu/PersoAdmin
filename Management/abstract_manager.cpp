#include "abstract_manager.h"
#include "global_environment.h"
#include "interaction_system.h"

AbstractManager::AbstractManager(const QString& name) : PObject{name} {
  connectDependencies();
}

AbstractManager::~AbstractManager() {}

void AbstractManager::initOperation(const QString& name) {
  sendLog(QString("Начало выполнения операции %1. ").arg(name));
  emit executionStarted(name);
}

void AbstractManager::processOperationError(const QString& name,
                                            ReturnStatus ret) {
  sendLog(QString("Не удалось выполнить операцию %1. ").arg(name));
  emit executionFinished(name, ret);
}

void AbstractManager::completeOperation(const QString& name) {
  sendLog(QString("Операция %1 успешно выполнена. ").arg(name));
  emit executionFinished(name, ReturnStatus::NoError);
}

void AbstractManager::connectDependencies() {
  const InteractionSystem* is = static_cast<const InteractionSystem*>(
      GlobalEnvironment::instance()->getObject("InteractionSystem"));

  connect(this, &AbstractManager::executionStarted, is,
          &InteractionSystem::processOperationStart);
  connect(this, &AbstractManager::executionFinished, is,
          &InteractionSystem::processOperationFinish);
}
