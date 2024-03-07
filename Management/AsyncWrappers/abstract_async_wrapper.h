#ifndef ABSTRACTASYNCWRAPPER_H
#define ABSTRACTASYNCWRAPPER_H

#include "loggable_object.h"
#include "named_object.h"
#include "types.h"

class AbstractAsyncWrapper : public NamedObject, public LoggableObject {
  Q_OBJECT
 public:
  explicit AbstractAsyncWrapper(const QString& name);
  virtual ~AbstractAsyncWrapper();

 protected:
  virtual void initOperation(const QString& name);
  virtual void processOperationError(const QString& name, ReturnStatus ret);
  virtual void completeOperation(const QString& name);

 private:
  void connectDependencies(void);

 signals:
  void executionStarted(const QString& opname);
  void executionFinished(const QString& opname);
  void executionStatus(ReturnStatus ret);
};

#endif // ABSTRACTASYNCWRAPPER_H
