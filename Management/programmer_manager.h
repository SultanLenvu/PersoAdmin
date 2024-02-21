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
 public slots:
  virtual void onInstanceThreadStarted() override;
};

#endif // PROGRAMMERMANAGER_H
