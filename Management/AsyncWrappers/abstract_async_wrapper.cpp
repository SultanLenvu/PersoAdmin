#include "abstract_async_wrapper.h"
#include "global_environment.h"
#include "progress_indicator.h"

AbstractAsyncWrapper::AbstractAsyncWrapper(const QString& name)
    : PObject{name} {
  connectDependencies();
}

AbstractAsyncWrapper::~AbstractAsyncWrapper() {}

void AbstractAsyncWrapper::initOperation(const QString& name) {
  sendLog(QString("Начало выполнения операции %1. ").arg(name));
  emit executionStarted(name);
}

void AbstractAsyncWrapper::processOperationError(const QString& name,
                                                 ReturnStatus ret) {
  sendLog(QString("Не удалось выполнить операцию %1. ").arg(name));
  emit executionFinished(name);
  emit executionStatus(ret);
}

void AbstractAsyncWrapper::completeOperation(const QString& name) {
  sendLog(QString("Операция %1 успешно выполнена. ").arg(name));
  emit executionFinished(name);
  emit executionStatus(ReturnStatus::NoError);
}

void AbstractAsyncWrapper::connectDependencies() {
  ProgressIndicator* is = static_cast<ProgressIndicator*>(
      GlobalEnvironment::instance()->getObject("ProgressIndicator"));

  connect(this, &AbstractAsyncWrapper::executionStarted, is,
          &ProgressIndicator::begin);
  connect(this, &AbstractAsyncWrapper::executionFinished, is,
          &ProgressIndicator::finish);
}
