#ifndef ABSTRACTASYNCWRAPPER_H
#define ABSTRACTASYNCWRAPPER_H

#include <pobject.h>
#include "types.h"

class AbstractAsyncWrapper : public PObject
{
  Q_OBJECT
 public:
  explicit AbstractAsyncWrapper(const QString& name);
  virtual ~AbstractAsyncWrapper();

 public slots:
  virtual void onInstanceThreadStarted(void) = 0;

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
