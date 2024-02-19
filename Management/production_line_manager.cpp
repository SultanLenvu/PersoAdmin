#include "production_line_manager.h"

ProductionLineManager::ProductionLineManager(
    const QString& name,
    std::shared_ptr<AbstractSqlDatabase> database)
    : AbstractManager(name), Database(database) {
  loadSettings();
}

ProductionLineManager::~ProductionLineManager() {}

void ProductionLineManager::onInstanceThreadStarted() {}

void ProductionLineManager::applySettings() {
  sendLog("Применение новых настроек.");

  loadSettings();
}

void ProductionLineManager::create(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("create");

  if (!Database->openTransaction()) {
    processOperationError("create", ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    processOperationError("deactivateAll", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("create", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("activateAll");
}

void ProductionLineManager::activate(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("activate");

  if (!Database->openTransaction()) {
    processOperationError("activate", ReturnStatus::DatabaseTransactionError);
    return;
  }

  SqlQueryValues productionLineNewValues;
  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines",
                               QString("id = %1").arg((*param)["id"]),
                               productionLineNewValues)) {
    Database->rollbackTransaction();
    processOperationError("activate", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("activate", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("activate");
}

void ProductionLineManager::activateAll() {
  initOperation("activateAll");

  if (!Database->openTransaction()) {
    processOperationError("activateAll",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  SqlQueryValues productionLineNewValues;
  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines", productionLineNewValues)) {
    Database->rollbackTransaction();
    processOperationError("activateAll", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("activateAll",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("activateAll");
}

void ProductionLineManager::deactivate(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("deactivate");

  if (!Database->openTransaction()) {
    processOperationError("deactivate", ReturnStatus::DatabaseTransactionError);
    return;
  }

  SqlQueryValues productionLineNewValue;
  productionLineNewValue.add("in_process", "false");
  productionLineNewValue.add("launched", "false");
  productionLineNewValue.add("completed", "false");
  productionLineNewValue.add("active", "false");
  productionLineNewValue.add("box_id", "NULL");
  productionLineNewValue.add("transponder_id", "NULL");
  if (!Database->updateRecords("production_lines",
                               QString("id = %1").arg((*param)["id"]),
                               productionLineNewValue)) {
    Database->rollbackTransaction();
    processOperationError("deactivate", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("deactivate", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("deactivate");
}

void ProductionLineManager::deactivateAll() {
  initOperation("deactivateAll");

  if (!Database->openTransaction()) {
    processOperationError("deactivateAll",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    processOperationError("deactivateAll", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("deactivateAll",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("deactivateAll");
}

void ProductionLineManager::edit(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("edit");

  if (!Database->openTransaction()) {
    processOperationError("edit", ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("edit", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("edit");
}

void ProductionLineManager::remove(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("remove");

  if (!Database->openTransaction()) {
    processOperationError("remove", ReturnStatus::DatabaseTransactionError);
    return;
  }

  //  if (!removeLastProductionLine()) {
  //    sendLog("Получена ошибка при удалении последней линии производства. ");
  //    Database->rollbackTransaction();
  //    return ReturnStatus::DatabaseQueryError;
  //  }

  if (!Database->commitTransaction()) {
    processOperationError("remove", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("remove");
}

void ProductionLineManager::get(const QString& name) {
  initOperation("get");

  completeOperation("get");
}

void ProductionLineManager::loadSettings() {}

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
