#include "database_manager.h"
#include "postgre_sql_database.h"
#include "sql_query_values.h"

DatabaseManager::DatabaseManager(const QString& name) : AbstractManager(name) {}

DatabaseManager::~DatabaseManager() {}

void DatabaseManager::onInstanceThreadStarted() {
  createDatabase();
}

void DatabaseManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Database->applySettings();
}

std::shared_ptr<AbstractSqlDatabase> DatabaseManager::database() {
  return Database;
}

void DatabaseManager::connect() {
  initOperation("connect");

  if (!Database->connect()) {
    processOperationError("connect", ReturnStatus::DatabaseConnectionError);
  }

  completeOperation("connect");
}

void DatabaseManager::disconnect() {
  initOperation("connect");

  Database->disconnect();

  completeOperation("connect");
}

void DatabaseManager::getTable(const QString& name) {
  initOperation("getTable");

  std::shared_ptr<SqlQueryValues> response(new SqlQueryValues());

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);

  if (!Database->readRecords(name, *response)) {
    processOperationError("getTable", ReturnStatus::DatabaseConnectionError);
    return;
  }

  // Отправляем ответ
  emit responseReady(response);

  completeOperation("getTable");
}

void DatabaseManager::execCustomRequest(const QString& req) {
  initOperation("execCustomRequest");

  std::shared_ptr<SqlQueryValues> response(new SqlQueryValues());

  if (!Database->openTransaction()) {
    processOperationError("execCustomRequest",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->execCustomRequest(req, *response)) {
    processOperationError("execCustomRequest",
                          ReturnStatus::DatabaseQueryError);
    Database->rollbackTransaction();
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("execCustomRequest",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  // Отправляем ответ
  emit responseReady(response);

  completeOperation("execCustomRequest");
}

void DatabaseManager::createDatabase() {
  Database = std::shared_ptr<AbstractSqlDatabase>(
      new PostgreSqlDatabase("PostgreSqlDatabase"));
}

void DatabaseManager::loadSettings() {
  QSettings settings;
}
