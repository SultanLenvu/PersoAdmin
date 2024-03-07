#include "abstract_async_wrapper.h"
#include "global_environment.h"
#include "measuring_progress_indicator.h"

AbstractAsyncWrapper::AbstractAsyncWrapper(const QString& name)
    : NamedObject{name} {
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
  MeasuringProgressIndicator* is = static_cast<MeasuringProgressIndicator*>(
      GlobalEnvironment::instance()->getObject("MeasuringProgressIndicator"));

  connect(this, &AbstractAsyncWrapper::executionStarted, is,
          &MeasuringProgressIndicator::begin);
  connect(this, &AbstractAsyncWrapper::executionFinished, is,
          &MeasuringProgressIndicator::finish);
}
