#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include "psobject.h"
#include "types.h"

class AbstractManager : public PSObject {
  Q_OBJECT
 public:
  enum Type {
    Database,
    Order,
    ProductionLine,
    PersoServer,
    Programmer,
    StickerPrinter,
  };
  Q_ENUM(Type)

 public:
  explicit AbstractManager(const QString& name);
  virtual ~AbstractManager();

 public:
  virtual Type type() const = 0;

 public slots:
  virtual void onInstanceThreadStarted(void) = 0;
  virtual void applySettings() = 0;

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
