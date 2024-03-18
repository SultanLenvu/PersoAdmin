#ifndef PROGRESSABLEASYNCWRAPPER_H
#define PROGRESSABLEASYNCWRAPPER_H

#include "i_async_wrapper.h"
#include "progress_indicator_link.h"
#include "status_indicator_link.h"

class ProgressableAsyncWrapper : public IAsyncWrapper {
 private:
  StatusIndicatorLink SIL;
  ProgressIndicatorLink PIB;

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
