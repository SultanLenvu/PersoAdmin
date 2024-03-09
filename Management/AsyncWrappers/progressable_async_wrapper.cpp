#include "progressable_async_wrapper.h"

void ProgressableAsyncWrapper::initOperation(const QString& name) {
  PIB.begin(name);
}

void ProgressableAsyncWrapper::processOperationError(const QString& name,
                                                     ReturnStatus ret) {
  PIB.finish(name);
  RSB.handle(ret);
}

void ProgressableAsyncWrapper::completeOperation(const QString& name) {
  PIB.finish(name);
  RSB.handle(ReturnStatus::NoError);
}
