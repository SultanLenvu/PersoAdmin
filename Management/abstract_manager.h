#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include "pobject.h"
#include "types.h"

class AbstractManager : public PObject {
  Q_OBJECT
 public:
  explicit AbstractManager(const QString& name);
  virtual ~AbstractManager();

 public slots:
  virtual void onInstanceThreadStarted(void) = 0;

 protected:
  virtual void initOperation(const QString& name);
  virtual void processOperationError(const QString& name, ReturnStatus ret);
  virtual void completeOperation(const QString& name);

 private:
  void connectDependencies(void);

 signals:
  void executionStarted(const QString& opName);
  void executionFinished(const QString& opName, ReturnStatus ret);
};

#endif  // ABSTRACTMANAGER_H
