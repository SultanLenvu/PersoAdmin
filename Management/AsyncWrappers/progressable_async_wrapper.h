#ifndef PROGRESSABLEASYNCWRAPPER_H
#define PROGRESSABLEASYNCWRAPPER_H

#include "i_async_wrapper.h"
#include "loggable_object.h"
#include "named_object.h"

class ProgressableAsyncWrapper : public NamedObject,
                                 public IAsyncWrapper,
                                 public LoggableObject {
  Q_OBJECT

 public:
  explicit ProgressableAsyncWrapper(const QString& name);
  virtual ~ProgressableAsyncWrapper() = default;

 protected:
  virtual void initOperation(const QString& name);
  virtual void processOperationError(const QString& name, ReturnStatus ret);
  virtual void completeOperation(const QString& name);

 private:
  Q_DISABLE_COPY_MOVE(ProgressableAsyncWrapper)
  void connectDependecies(void);

 signals:
  void returnStatusReady(ReturnStatus ret);
  void executionStarted(const QString& name);
  void executionFinished(const QString& name);
};

#endif // PROGRESSABLEASYNCWRAPPER_H
