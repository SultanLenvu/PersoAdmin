#include "progressable_async_wrapper.h"
#include "global_environment.h"
#include "progress_indicator.h"
#include "status_indicator.h"

ProgressableAsyncWrapper::ProgressableAsyncWrapper(const QString& name)
    : NamedObject(name), LoggableObject(name) {
  connectDependecies();
}

void ProgressableAsyncWrapper::initOperation(const QString& name) {
  sendLog(QString("Начало выполнения операции %1.").arg(name));
  emit executionStarted(name);
}

void ProgressableAsyncWrapper::processOperationError(const QString& name,
                                                     ReturnStatus ret) {
  sendLog(QString("Получена ошибка при выполнении операции %1.").arg(name));
  emit executionFinished(name);
  emit returnStatusReady(ret);
}

void ProgressableAsyncWrapper::completeOperation(const QString& name) {
  sendLog(QString("Операция %1 успешно выполнена.").arg(name));
  emit executionFinished(name);
  emit returnStatusReady(ReturnStatus::NoError);
}

void ProgressableAsyncWrapper::connectDependecies() {
  const StatusIndicator* si =
      GlobalEnvironment::instance()->getObject<const StatusIndicator>(
          "StatusIndicator");

  connect(this, &ProgressableAsyncWrapper::returnStatusReady, si,
          &StatusIndicator::handle);

  const ProgressIndicator* is =
      GlobalEnvironment::instance()->getObject<const ProgressIndicator>(
          "ProgressIndicator");

  connect(this, &ProgressableAsyncWrapper::executionStarted, is,
          &ProgressIndicator::begin);
  connect(this, &ProgressableAsyncWrapper::executionFinished, is,
          &ProgressIndicator::finish);
}
