#include <QDateTime>
#include <QFile>

#include "definitions.h"
#include "order_manager.h"

OrderManager::OrderManager(const QString& name) : AbstractManager(name) {}

OrderManager::~OrderManager() {}

void OrderManager::onInstanceThreadStarted() {}

void OrderManager::applySettings() {}

void OrderManager::setDatabase(std::shared_ptr<AbstractSqlDatabase> database) {
  Database = database;
}

void OrderManager::create(const std::shared_ptr<StringDictionary> param) {
  initOperation("create");

  if (!Database->openTransaction()) {
    processOperationError("create", ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!addOrder(*param)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    Database->rollbackTransaction();
    processOperationError("create", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("create", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("create");
}

void OrderManager::startAssembling(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("startAssembling");

  if (!Database->openTransaction()) {
    processOperationError("startAssembling",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  SqlQueryValues orders;
  SqlQueryValues orderNewValue;

  // Проверяем, что в процессе сборки нет других заказов
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("orders", "in_process = true", orders)) {
    Database->rollbackTransaction();
    processOperationError("startAssembling",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  // В сборке может находится только один заказ
  if (!orders.isEmpty()) {
    Database->rollbackTransaction();
    processOperationError("startAssembling",
                          ReturnStatus::OrderMultiplyAssembly);
    return;
  }

  orderNewValue.add("in_process", "true");
  orderNewValue.add("assembling_start", QDateTime::currentDateTime().toString(
                                            POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("orders", QString("id = %1").arg((*param)["id"]),
                               orderNewValue)) {
    Database->rollbackTransaction();
    processOperationError("startAssembling", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("startAssembling",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("startAssembling");
}

void OrderManager::stopAssembling(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("stopAssembling");

  if (!Database->openTransaction()) {
    processOperationError("stopAssembling",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    processOperationError("stopAssembling", ReturnStatus::DatabaseQueryError);
    return;
  }

  SqlQueryValues orderNewValue;
  orderNewValue.add("in_process", "false");
  if (!Database->updateRecords("orders", QString("id = %1").arg((*param)["id"]),
                               orderNewValue)) {
    Database->rollbackTransaction();
    processOperationError("stopAssembling", ReturnStatus::DatabaseQueryError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("stopAssembling",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("stopAssembling");
}

void OrderManager::release(const std::shared_ptr<StringDictionary> param) {
  initOperation("release");

  if (!Database->openTransaction()) {
    processOperationError("release", ReturnStatus::DatabaseTransactionError);
    return;
  }

  QString table = param->value("table");

  if (table == "transponders") {
    ret = releaseTransponder(param->value("id"));
  } else if (table == "boxes") {
    ret = releaseBox(param->value("id"));
  } else if (table == "pallets") {
    ret = releasePallet(param->value("id"));
  } else if (table == "orders") {
    ret = releaseOrder(param->value("id"));
  } else {
    emit executionFinished("releaseTranspondersManually",
                           ReturnStatus::ParameterError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("release", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("release");
}

void OrderManager::refund(const std::shared_ptr<StringDictionary> param) {
  initOperation("refund");

  if (!Database->openTransaction()) {
    processOperationError("refund", ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("refund", ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("refund");
}

void OrderManager::generateShipmentRegister(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("generateShipmentRegister");

  completeOperation("generateShipmentRegister");
}

void OrderManager::initTransportMasterKeys() {
  initOperation("initTransportMasterKeys");

  if (!Database->openTransaction()) {
    processOperationError("initTransportMasterKeys",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("initTransportMasterKeys",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("initTransportMasterKeys");
}

void OrderManager::initIssuers() {
  initOperation("initIssuers");

  if (!Database->openTransaction()) {
    processOperationError("initIssuers",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("initIssuers",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("initIssuers");
}

void OrderManager::linkIssuerWithKeys(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("linkIssuerWithKeys");

  if (!Database->openTransaction()) {
    processOperationError("linkIssuerWithKeys",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  if (!Database->commitTransaction()) {
    processOperationError("linkIssuerWithKeys",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  completeOperation("linkIssuerWithKeys");
}

bool OrderManager::addOrder(const StringDictionary& param) {
  uint32_t transponderCount = param.value("transponder_quantity").toInt();
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  SqlQueryValues issuerRecord;
  SqlQueryValues lastAddedOrder;
  SqlQueryValues orderRecord;
  SqlQueryValues newOrderValues;
  int32_t lastId = 0;

  // Ищем заказчика
  if (!Database->readRecords(
          "issuers", QString("name = '%1'").arg(param.value("issuer_name")),
          issuerRecord)) {
    sendLog(QString("Получена ошибка при поиске эмитента \"%1\".")
                .arg(param.value("issuer_name")));
    return false;
  }

  // Получаем идентификатор последнего заказа
  lastId = getLastId("orders");
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор последнего заказа");
    return false;
  }

  // Формируем новую запись
  newOrderValues.add("id", QString::number(lastId + 1));
  newOrderValues.add("issuer_id", issuerRecord.get("id"));
  newOrderValues.add("quantity", QString::number(orderCapacity));
  newOrderValues.add("full_personalization",
                     param.value("full_personalization"));
  newOrderValues.add(
      "transponder_model",
      param.value("transponder_model")
          .rightJustified(TRANSPONDER_MODEL_CHAR_LENGTH, QChar(' ')));
  newOrderValues.add("accr_reference", param.value("accr_reference"));
  newOrderValues.add("equipment_class", param.value("equipment_class"));
  newOrderValues.add("manufacturer_id", param.value("manufacturer_id"));
  if (!Database->createRecords("orders", newOrderValues)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    return false;
  }

  // Добавляем паллеты в заказ
  return addPallets(newOrderValues.get("id"), param);
}

bool OrderManager::addPallets(const QString& orderId,
                              const StringDictionary& param) {
  uint32_t transponderCount = param.value("transponder_quantity").toInt();
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  SqlQueryValues newPalletValues;
  int32_t lastId = 0;

  // Получаем идентификатор последней палеты
  lastId = getLastId("pallets");
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор последней паллеты.");
    return false;
  }

  // Формируем новые записи
  for (uint32_t i = 1; i <= orderCapacity; i++) {
    newPalletValues.add("id", QString::number(lastId + i));
    newPalletValues.add("quantity", QString::number(palletCapacity));
    newPalletValues.add("order_id", orderId);
  }

  // Добавляем новые палеты
  if (!Database->createRecords("pallets", newPalletValues)) {
    sendLog(QString("Получена ошибка при добавлении палет в заказ %1. ")
                .arg(orderId));
    return false;
  }

  // Открываем PAN-файл
  QFile panFile(param.value("pan_file_path"));
  if (!panFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии PAN-файла. ");
    return false;
  }
  QTextStream panSource(&panFile);

  // Добавляем боксы в паллеты
  for (uint32_t i = 0; i < orderCapacity; i++) {
    if (!addBoxes(newPalletValues.get(i, "id"), param, panSource)) {
      sendLog(QString("Получена ошибка при добавлении боксов в паллету %1. ")
                  .arg(newPalletValues.get(i, "id")));
      return false;
    }
  }

  return true;
}

bool OrderManager::addBoxes(const QString& palletId,
                            const StringDictionary& param,
                            QTextStream& panSource) {
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  SqlQueryValues newBoxValues;
  int32_t lastId = 0;

  // Получаем идентификатор последнего  бокса
  lastId = getLastId("boxes");
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор последнего бокса.");
    return false;
  }

  // Создаем боксы
  for (uint32_t i = 1; i <= palletCapacity; i++) {
    newBoxValues.add("id", QString::number(lastId + i));
    newBoxValues.add("quantity", QString::number(boxCapacity));
    newBoxValues.add("pallet_id", palletId);
  }

  // Добавляем новые записи
  if (!Database->createRecords("boxes", newBoxValues)) {
    sendLog(QString("Получена ошибка при добавлении боксов в паллету %1.")
                .arg(palletId));
    return false;
  }

  for (uint32_t i = 0; i < palletCapacity; i++) {
    // Считываем PAN'ы
    std::shared_ptr<QVector<QString>> pans(new QVector<QString>());
    for (int32_t j = 0; j < boxCapacity && !panSource.atEnd(); j++) {
      pans->append(
          panSource.readLine().leftJustified(FULL_PAN_CHAR_LENGTH, QChar('F')));
    }
    if (!addTransponders(newBoxValues.get(i, "id"), pans, param)) {
      sendLog(
          QString("Получена ошибка при добавлении транспондеров в бокса %1. ")
              .arg(newBoxValues.get(i, "id")));
      return false;
    }
  }

  return true;
}

bool OrderManager::addTransponders(
    const QString& boxId,
    const std::shared_ptr<QVector<QString>>& pans,
    const StringDictionary& param) {
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  int32_t lastId = 0;
  SqlQueryValues newTransponders;

  // Получаем идентификатор последнего транспондера
  lastId = getLastId("transponders");
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор последнего транспондера.");
    return false;
  }

  // Создаем новые транспондеры
  newTransponders.add("personal_account_number", pans);
  for (uint32_t i = 1; i <= boxCapacity; i++) {
    // Формируем новую запись
    newTransponders.add("id", QString::number(lastId + i));
    newTransponders.add("release_counter", "0");
    newTransponders.add("box_id", boxId);
  }

  // Добавляем новые записи
  if (!Database->createRecords("transponders", newTransponders)) {
    sendLog(QString("Получена ошибка при добавлении транспондеров в бокс %1.")
                .arg(boxId));
    return false;
  }

  return true;
}

bool OrderManager::stopAllProductionLines() {
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

bool OrderManager::releaseTransponder(const QString& id) {
  Database->setRecordMaxCount(1);

  if (!Database->readRecords("transponders", "id = " + id, transponder)) {
    sendLog(
        QString("Получена ошибка при поиске данных транспондера %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (transponder.isEmpty()) {
    sendLog(QString("Транспондер %1 не найден. ").arg(id));
    return ReturnStatus::TransponderMissed;
  }

  transponderNewValues.add("release_counter", "1");
  transponderNewValues.add("ucid", "NULL");
  transponderNewValues.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "id = " + id,
                               transponderNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }
}

bool OrderManager::releaseBox(const QString& id) {}

bool OrderManager::releasePallet(const QString& id) {}

bool OrderManager::releaseOrder(const QString& id) {}

bool OrderManager::refundTransponder(const QString& id) {}

bool OrderManager::refundBox(const QString& id) {}

bool OrderManager::refundPallet(const QString& id) {}

bool OrderManager::refundOrder(const QString& id) {}

int32_t OrderManager::getLastId(const QString& table) {
  SqlQueryValues record;

  if (!Database->readLastRecord(table, record)) {
    sendLog(
        QString("Получена ошибка при поиске последней записи в таблице '%1'. ")
            .arg(table));
    return false;
  }
  if (record.isEmpty()) {
    if (table == "transponders") {
      return TRANSPONDER_ID_START_SHIFT;
    }
    if (table == "boxes") {
      return BOX_ID_START_SHIFT;
    }
    if (table == "pallets") {
      return PALLET_ID_START_SHIFT;
    }
    return 0;
  }

  return record.get("id").toInt();
}
