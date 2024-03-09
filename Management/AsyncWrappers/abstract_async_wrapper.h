#ifndef ABSTRACTASYNCWRAPPER_H
#define ABSTRACTASYNCWRAPPER_H

#include <QString>

#include "types.h"

class AbstractAsyncWrapper {
 public:
  explicit AbstractAsyncWrapper() = default;
  virtual ~AbstractAsyncWrapper() = default;

 protected:
  virtual void initOperation(const QString& name) = 0;
  virtual void processOperationError(const QString& name, ReturnStatus ret) = 0;
  virtual void completeOperation(const QString& name) = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractAsyncWrapper)
};

#endif // ABSTRACTASYNCWRAPPER_H
