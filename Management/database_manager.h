#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "abstract_manager.h"

class DatabaseManager final : public AbstractManager {
  Q_OBJECT
 public:
  explicit DatabaseManager(const QString& name);
  ~DatabaseManager();

  // AbstractManager interface
 public:
  virtual Type type() const override;

 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;

  // DatabaseManager interface
 public slots:
  void connect(void);
  void disconnect(void);
  void getTable(const QString& name);
  void execCustomRequest(const QString& req);
};

#endif  // DATABASEMANAGER_H
