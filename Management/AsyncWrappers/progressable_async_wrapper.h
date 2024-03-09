#ifndef PROGRESSABLEASYNCWRAPPER_H
#define PROGRESSABLEASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "progress_indicator_bundle.h"
#include "return_status_handler_bundle.h"

class ProgressableAsyncWrapper : public AbstractAsyncWrapper {
 private:
  ReturnStatusHandlerBundle RSB;
  ProgressIndicatorBundle PIB;

 public:
  explicit ProgressableAsyncWrapper() = default;
  virtual ~ProgressableAsyncWrapper() = default;

 protected:
  virtual void initOperation(const QString& name);
  virtual void processOperationError(const QString& name, ReturnStatus ret);
  virtual void completeOperation(const QString& name);

 private:
  Q_DISABLE_COPY_MOVE(ProgressableAsyncWrapper)
};

#endif // PROGRESSABLEASYNCWRAPPER_H
