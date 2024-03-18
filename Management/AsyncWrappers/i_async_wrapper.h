#ifndef ABSTRACTASYNCWRAPPER_H
#define ABSTRACTASYNCWRAPPER_H

#include <QString>
#include "types.h"

class IAsyncWrapper {
 public:
  explicit IAsyncWrapper() = default;
  virtual ~IAsyncWrapper() = default;

 protected:
  virtual void initOperation(const QString& name) = 0;
  virtual void processOperationError(const QString& name, ReturnStatus ret) = 0;
  virtual void completeOperation(const QString& name) = 0;

 private:
  Q_DISABLE_COPY_MOVE(IAsyncWrapper)
};

#endif // ABSTRACTASYNCWRAPPER_H
