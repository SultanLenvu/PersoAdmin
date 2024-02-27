#include "production_line_manager.h"
#include "database_async_wrapper.h"
#include "global_environment.h"

ProductionLineManager::ProductionLineManager(const QString& name)
    : AbstractManager(name) {
  connectDependencies();
}

ProductionLineManager::~ProductionLineManager() {}

void ProductionLineManager::applyDatabase(
    std::shared_ptr<AbstractSqlDatabase> database) {
  Database = database;
}

ReturnStatus ProductionLineManager::create(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::activate(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  SqlQueryValues productionLineNewValues;
  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines",
                               QString("id = %1").arg(param["id"]),
                               productionLineNewValues)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::activateAll() {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  SqlQueryValues productionLineNewValues;
  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines", productionLineNewValues)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::deactivate(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  SqlQueryValues productionLineNewValue;
  productionLineNewValue.add("in_process", "false");
  productionLineNewValue.add("launched", "false");
  productionLineNewValue.add("completed", "false");
  productionLineNewValue.add("active", "false");
  productionLineNewValue.add("box_id", "NULL");
  productionLineNewValue.add("transponder_id", "NULL");
  if (!Database->updateRecords("production_lines",
                               QString("id = %1").arg(param["id"]),
                               productionLineNewValue)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::deactivateAll() {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::edit(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus ProductionLineManager::remove(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  //  if (!removeLastProductionLine()) {
  //    sendLog("Получена ошибка при удалении последней линии производства. ");
  //    Database->rollbackTransaction();
  //    return ReturnStatus::DatabaseQueryError;
  //  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

void ProductionLineManager::connectDependencies() {
  DatabaseAsyncWrapper* dm = static_cast<DatabaseAsyncWrapper*>(
      GlobalEnvironment::instance()->getObject("DatabaseAsyncWrapper"));

  connect(dm, &DatabaseAsyncWrapper::databaseCreated, this,
          &ProductionLineManager::applyDatabase);
}

bool ProductionLineManager::addProductionLine(const StringDictionary& param) {
  SqlQueryValues newProductionLine;
  int32_t lastId = 0;

  if (!Database->getLastId("production_lines", lastId)) {
    sendLog("Получена ошибка при получении последнего идентификатора. ");
    return false;
  }
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор записи.");
    return false;
  }

  // Создаем новую линию производства
  newProductionLine.add("login", param.value("login"));
  newProductionLine.add("password", param.value("password"));
  newProductionLine.add("name", param.value("name"));
  newProductionLine.add("surname", param.value("surname"));
  newProductionLine.add("id", QString::number(lastId + 1));
  newProductionLine.add("box_id", "NULL");
  newProductionLine.add("in_process", "false");

  // Добавляем новую запись
  if (!Database->createRecords("production_lines", newProductionLine)) {
    sendLog("Получена ошибка при добавлении линии производства. ");
    return false;
  }

  return true;
}

bool ProductionLineManager::stopAllProductionLines() {
  SqlQueryValues productionLineNewValues;

  productionLineNewValues.add("in_process", "false");
  productionLineNewValues.add("launched", "false");
  productionLineNewValues.add("active", "false");
  productionLineNewValues.add("box_id", "NULL");
  productionLineNewValues.add("transponder_id", "NULL");
  if (!Database->updateRecords("production_lines", productionLineNewValues)) {
    sendLog(
        QString("Получена ошибка при обновлении данных линии производства."));
    return false;
  }

  return true;
}
