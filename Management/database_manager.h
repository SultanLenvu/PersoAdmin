#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "abstract_manager.h"
#include "abstract_sql_database.h"
#include "types.h"

class DatabaseManager final : public AbstractManager {
  Q_OBJECT
 private:
  std::shared_ptr<AbstractSqlDatabase> Database;

 public:
  explicit DatabaseManager(const QString& name);
  ~DatabaseManager();

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;

  // Own
 public:
  std::shared_ptr<AbstractSqlDatabase> database(void);

 public slots:
  void connect(void);
  void disconnect(void);
  void getTable(const QString& name);
  void execCustomRequest(const QString& req);

 private:
  void createDatabase(void);

  void loadSettings(void);
  void createOperationNames(void);

 signals:
  void responseReady(const std::shared_ptr<SqlQueryValues> response);
};

#endif  // DATABASEMANAGER_H
