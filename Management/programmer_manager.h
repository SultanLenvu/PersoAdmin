#ifndef PROGRAMMERMANAGER_H
#define PROGRAMMERMANAGER_H

#include "abstract_manager.h"
#include <QWidget>

class ProgrammerManager : public AbstractManager
{
  Q_OBJECT
 public:
  explicit ProgrammerManager(const QString& name);
  ~ProgrammerManager();

  // AbstractManager interface
 public:
  virtual Type type() const override;

 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;
};

#endif // PROGRAMMERMANAGER_H
