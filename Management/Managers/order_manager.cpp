#include <QDateTime>
#include <QFile>
#include <QSettings>

#include "definitions.h"
#include "order_manager.h"

OrderManager::OrderManager(const QString& name,
                           std::shared_ptr<ISqlDatabase> database)
    : NamedObject(name), LoggableObject(name), Database(database) {
  doLoadSettings();
}

ReturnStatus OrderManager::create(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!addOrder(param)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::startAssembling(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  SqlQueryValues orders;
  SqlQueryValues orderNewValue;

  // Проверяем, что в процессе сборки нет других заказов
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("orders", "in_process = true", orders)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  // В сборке может находится только один заказ
  if (!orders.isEmpty()) {
    Database->rollbackTransaction();
    return ReturnStatus::OrderMultiplyAssembly;
  }

  orderNewValue.add("in_process", "true");
  orderNewValue.add("assembling_start", QDateTime::currentDateTime().toString(
                                            POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("orders", QString("id = %1").arg(param["id"]),
                               orderNewValue)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::stopAssembling(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  SqlQueryValues orderNewValue;
  orderNewValue.add("in_process", "false");
  if (!Database->updateRecords("orders", QString("id = %1").arg(param["id"]),
                               orderNewValue)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::generateShipmentRegister(
    const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  QString ShipmentRegisterName =
      QString("sr_pallets_%1_%2.csv")
          .arg(param.value("first_pallet_id"), param.value("last_pallet_id"));
  QFile file(ShipmentRegisterName);
  QTextStream out(&file);

  // Открываем файл для записи
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return ReturnStatus::RegisterFileError;
  }
  file.resize(0);
  out << "model;sn;pan;box_id;pallet_id\n";

  for (uint32_t i = param.value("first_pallet_id").toUInt();
       i <= param.value("last_pallet_id").toUInt(); i++) {
    sendLog(QString("Отгрузка паллеты %1.").arg(QString::number(i)));
    ret = shipPallet(QString::number(i), out);
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при отгрузке паллеты %1.")
                  .arg(QString::number(i)));
      file.close();
      return ReturnStatus::DatabaseQueryError;
    }
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::release(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  QString table = param.value("table");
  ReturnStatus ret = ReturnStatus::NoError;

  if (table == "transponders") {
    ret = releaseTransponder(param.value("id"));
  } else if (table == "boxes") {
    ret = releaseBox(param.value("id"));
  } else if (table == "pallets") {
    ret = releasePallet(param.value("id"));
  } else if (table == "orders") {
    ret = releaseOrder(param.value("id"));
  } else {
    return ReturnStatus::ParameterError;
  }

  if (ret != ReturnStatus::NoError) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::refund(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  QString table = param.value("table");
  ReturnStatus ret = ReturnStatus::NoError;

  if (table == "transponders") {
    ret = refundTransponder(param.value("id"));
  } else if (table == "boxes") {
    ret = refundBox(param.value("id"));
  } else if (table == "pallets") {
    ret = refundPallet(param.value("id"));
  } else if (table == "orders") {
    ret = refundOrder(param.value("id"));
  } else {
    return ReturnStatus::ParameterError;
  }

  if (ret != ReturnStatus::NoError) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::initTransportMasterKeys() {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  SqlQueryValues transportKeys;

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  // Конструируем запись
  transportKeys.add("id", "1");
  transportKeys.add("accr_key", TRANSPORT_MACCRKEY_DEFAULT_VALUE);
  transportKeys.add("per_key", TRANSPORT_MPERKEY_DEFAULT_VALUE);
  transportKeys.add("au_key1", TRANSPORT_MAUKEY1_DEFAULT_VALUE);
  transportKeys.add("au_key2", TRANSPORT_MAUKEY2_DEFAULT_VALUE);
  transportKeys.add("au_key3", TRANSPORT_MAUKEY3_DEFAULT_VALUE);
  transportKeys.add("au_key4", TRANSPORT_MAUKEY4_DEFAULT_VALUE);
  transportKeys.add("au_key5", TRANSPORT_MAUKEY5_DEFAULT_VALUE);
  transportKeys.add("au_key6", TRANSPORT_MAUKEY6_DEFAULT_VALUE);
  transportKeys.add("au_key7", TRANSPORT_MAUKEY7_DEFAULT_VALUE);
  transportKeys.add("au_key8", TRANSPORT_MAUKEY8_DEFAULT_VALUE);

  if (!Database->createRecords("transport_master_keys", transportKeys)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::initIssuers() {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  SqlQueryValues newValues;
  int32_t id = 0;

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  newValues.add("id", QString::number(++id));
  newValues.add("name", "Пауэр Синтез");
  newValues.add("efc_context_mark", "000000000001");
  newValues.add("transport_master_keys_id", "1");

  newValues.add("id", QString::number(++id));
  newValues.add("name", "Автодор");
  newValues.add("efc_context_mark", "570002FF0070");
  newValues.add("transport_master_keys_id", "1");

  newValues.add("id", QString::number(++id));
  newValues.add("name", "Новое качество дорог");
  newValues.add("efc_context_mark", "000000000001");
  newValues.add("transport_master_keys_id", "1");

  newValues.add("id", QString::number(++id));
  newValues.add("name", "Магистраль северной столицы");
  newValues.add("efc_context_mark", "570001FF0070");
  newValues.add("transport_master_keys_id", "1");

  newValues.add("id", QString::number(++id));
  newValues.add("name", "Объединенные системы сбора платы");
  newValues.add("efc_context_mark", "000000000001");
  newValues.add("transport_master_keys_id", "1");

  if (!Database->createRecords("issuers", newValues)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::linkIssuerWithKeys(const StringDictionary& param) {
  if (!Database) {
    sendLog("База данных не определена.");
    return ReturnStatus::DatabaseMissed;
  }

  SqlQueryValues issuerNewValue;

  if (!Database->openTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!Database->updateRecords("issuers", "id = " + param.value("issuer_id"),
                               issuerNewValue)) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

void OrderManager::loadSettings() {
  sendLog("Загрузка настроек.");
  doLoadSettings();
}

void OrderManager::doLoadSettings() {
  QSettings settings;

  ShipmentRegisterDir = "/ShipmentRegisters/";
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

  if (!Database->getLastId("orders", lastId)) {
    sendLog("Получена ошибка при получении последнего идентификатора. ");
    return false;
  }
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор записи.");
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

  if (!Database->getLastId("pallets", lastId)) {
    sendLog("Получена ошибка при получении последнего идентификатора. ");
    return false;
  }
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор записи.");
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

  if (!Database->getLastId("boxes", lastId)) {
    sendLog("Получена ошибка при получении последнего идентификатора. ");
    return false;
  }
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор записи.");
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

  if (!Database->getLastId("transponders", lastId)) {
    sendLog("Получена ошибка при получении последнего идентификатора. ");
    return false;
  }
  if (lastId == 0) {
    sendLog("Получен некорректный идентификатор записи.");
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

ReturnStatus OrderManager::releaseTransponder(const QString& id) {
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValues;

  Database->setRecordMaxCount(1);

  if (!Database->readRecords("transponders", QString("id = %1").arg(id),
                             transponder)) {
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
  if (!Database->updateRecords("transponders", QString("id = %1").arg(id),
                               transponderNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::releaseBox(const QString& id) {
  SqlQueryValues box;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;
  SqlQueryValues palletNewValues;

  if (!Database->readRecords("boxes", QString("id = %1").arg(id), box)) {
    sendLog(QString("Получена ошибка при поиске данных бокса %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (box.isEmpty()) {
    sendLog(QString("Бокс %1 не найден. ").arg(id));
    return ReturnStatus::BoxMissed;
  }

  transponderNewValues.add("release_counter", "1");
  transponderNewValues.add("ucid", "NULL");
  transponderNewValues.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "box_id = " + id,
                               transponderNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  boxNewValues.add("assembled_units", box.get("quantity"));
  boxNewValues.add("assembling_start", "NULL");
  boxNewValues.add("assembling_end", "NULL");
  if (!Database->updateRecords("boxes", QString("id = %1").arg(id),
                               boxNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::releasePallet(const QString& id) {
  SqlQueryValues pallet;
  SqlQueryValues palletNewValue;
  SqlQueryValues boxes;
  ReturnStatus ret;

  if (!Database->readRecords("pallets", QString("id = %1").arg(id), pallet)) {
    sendLog(QString("Получена ошибка при поиске данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (pallet.isEmpty()) {
    sendLog(QString("Паллета %1 не найдена. ").arg(id));
    return ReturnStatus::PalletMissed;
  }

  Database->setRecordMaxCount(0);
  if (!Database->readRecords("boxes", "pallet_id = " + id, boxes)) {
    sendLog(
        QString("Получена ошибка при поиске боксов в паллете %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (uint32_t i = 0; i < boxes.recordCount(); i++) {
    ret = releaseBox(boxes.get(i, "id"));
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при возврате бокса %1")
                  .arg(boxes.get(i, "id")));
      return ret;
    }
  }

  palletNewValue.add("assembled_units", pallet.get("quantity"));
  palletNewValue.add("assembling_start", "NULL");
  palletNewValue.add("assembling_end", "NULL");
  palletNewValue.add("shipped", "false");
  if (!Database->updateRecords("pallets", QString("id = %1").arg(id),
                               palletNewValue)) {
    sendLog(
        QString("Получена ошибка при изменении данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::releaseOrder(const QString& id) {
  SqlQueryValues order;
  SqlQueryValues orderNewValues;
  SqlQueryValues pallets;
  ReturnStatus ret;

  if (!Database->readRecords("orders", QString("id = %1").arg(id), order)) {
    sendLog(QString("Получена ошибка при поиске данных заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (order.isEmpty()) {
    sendLog(QString("Заказ %1 не найден. ").arg(id));
    return ReturnStatus::PalletMissed;
  }

  Database->setRecordMaxCount(0);
  if (!Database->readRecords("pallets", "order_id = " + id, pallets)) {
    sendLog(
        QString("Получена ошибка при поиске паллет из заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (uint32_t i = 0; i < pallets.recordCount(); i++) {
    ret = releasePallet(pallets.get(i, "id"));
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при возврате паллеты %1")
                  .arg(pallets.get(i, "id")));
      return ret;
    }
  }

  orderNewValues.add("assembled_units", order.get("quantity"));
  orderNewValues.add("shipped_units", "0");
  orderNewValues.add("assembling_start", "NULL");
  orderNewValues.add("assembling_end", "NULL");
  if (!Database->updateRecords("orders", QString("id = %1").arg(id),
                               orderNewValues)) {
    sendLog(
        QString("Получена ошибка при изменении данных заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::refundTransponder(const QString& id) {
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;

  if (!Database->readRecords("transponders", QString("id = %1").arg(id),
                             transponder)) {
    sendLog(
        QString("Получена ошибка при поиске данных транспондера %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (transponder.isEmpty()) {
    sendLog(QString("Транспондер %1 не найден. ").arg(id));
    return ReturnStatus::TransponderMissed;
  }

  transponderNewValues.add("release_counter", "0");
  transponderNewValues.add("ucid", "NULL");
  transponderNewValues.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", QString("id = %1").arg(id),
                               transponderNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::refundBox(const QString& id) {
  SqlQueryValues box;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;

  if (!Database->readRecords("boxes", QString("id = %1").arg(id), box)) {
    sendLog(QString("Получена ошибка при поиске данных бокса %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (box.isEmpty()) {
    sendLog(QString("Бокс %1 не найден. ").arg(id));
    return ReturnStatus::BoxMissed;
  }

  transponderNewValues.add("release_counter", "0");
  transponderNewValues.add("ucid", "NULL");
  transponderNewValues.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "box_id = " + id,
                               transponderNewValues)) {
    sendLog(
        QString(
            "Получена ошибка при изменении данных транспондеров из бокса %1. ")
            .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  boxNewValues.add("assembled_units", "0");
  boxNewValues.add("assembling_start", "NULL");
  boxNewValues.add("assembling_end", "NULL");
  boxNewValues.add("completed", "false");
  boxNewValues.add("production_line_id", "NULL");
  if (!Database->updateRecords("boxes", QString("id = %1").arg(id),
                               boxNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных бокса %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::refundPallet(const QString& id) {
  SqlQueryValues pallet;
  SqlQueryValues palletNewValue;
  SqlQueryValues boxes;
  ReturnStatus ret;

  if (!Database->readRecords("pallets", QString("id = %1").arg(id), pallet)) {
    sendLog(QString("Получена ошибка при поиске данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (pallet.isEmpty()) {
    sendLog(QString("Паллета %1 не найдена. ").arg(id));
    return ReturnStatus::PalletMissed;
  }

  Database->setRecordMaxCount(0);
  if (!Database->readRecords("boxes", "pallet_id = " + id, boxes)) {
    sendLog(
        QString("Получена ошибка при поиске боксов из паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (uint32_t i = 0; i < boxes.recordCount(); i++) {
    ret = refundBox(boxes.get(i, "id"));
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при возврате бокса %1")
                  .arg(boxes.get(i, "id")));
      return ret;
    }
  }

  palletNewValue.add("assembled_units", "0");
  palletNewValue.add("assembling_start", "NULL");
  palletNewValue.add("assembling_end", "NULL");
  palletNewValue.add("shipped", "false");
  if (!Database->updateRecords("pallets", QString("id = %1").arg(id),
                               palletNewValue)) {
    sendLog(
        QString("Получена ошибка при изменении данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::refundOrder(const QString& id) {
  SqlQueryValues order;
  SqlQueryValues orderNewValues;
  SqlQueryValues pallets;
  ReturnStatus ret;

  if (!Database->readRecords("orders", "id = " + id, order)) {
    sendLog(QString("Получена ошибка при поиске данных заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (order.isEmpty()) {
    sendLog(QString("Заказ %1 не найден. ").arg(id));
    return ReturnStatus::PalletMissed;
  }

  Database->setRecordMaxCount(0);
  if (!Database->readRecords("pallets", "order_id = " + id, pallets)) {
    sendLog(
        QString("Получена ошибка при поиске паллет из заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (uint32_t i = 0; i < pallets.recordCount(); i++) {
    ret = refundPallet(pallets.get(i, "id"));
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при возврате паллеты %1")
                  .arg(pallets.get(i, "id")));
      return ret;
    }
  }

  orderNewValues.add("assembled_units", "0");
  orderNewValues.add("shipped_units", "0");
  orderNewValues.add("assembling_start", "NULL");
  orderNewValues.add("assembling_end", "NULL");
  if (!Database->updateRecords("orders", "id = " + id, orderNewValues)) {
    sendLog(
        QString("Получена ошибка при изменении данных заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus OrderManager::shipPallet(const QString& id,
                                      QTextStream& registerOut) {
  QStringList tables{"transponders", "boxes", "pallets", "orders"};
  SqlQueryValues records;
  uint32_t quantity;

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);

  records.addField("transponder_model");
  records.addField("personal_account_number");
  records.addField("transponders.id");
  records.addField("box_id");
  records.addField("pallet_id");
  records.addField("boxes.assembling_start");
  records.addField("manufacturer_id");
  if (!Database->readMergedRecords(tables, QString("pallets.id = %1").arg(id),
                                   records)) {
    sendLog(
        QString("Получена ошибка поиске транспондеров в паллете %1.").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (int32_t i = 0; i < records.recordCount(); i++) {
    // Удаляем пробелы из названия модели
    QString tempModel = records.get(i, "transponder_model").remove(" ");

    // Преобразуем в десятичный формат
    QString manufacturerId =
        QString::number(records.get(i, "manufacturer_id").toInt(nullptr, 16));

    // Дата сборки
    QStringList tempDate = records.get(i, "boxes.assembling_start").split("T");
    QDate date = QDate::fromString(tempDate.first(), POSTGRES_DATE_TEMPLATE);
    QString batteryInsertationDate =
        QString("%1%2")
            .arg(QString::number(date.weekNumber()), 2, QChar('0'))
            .arg(QString::number(date.year() % 100), 2, QChar('0'));

    // Дополняем серийник до 10 цифр нулями слева
    QString extendedTransponderId =
        QString("%1").arg(records.get(i, "id"), 10, QChar('0'));

    // Конструируем серийный номер транспондера
    QString sn = QString("%1%2%3").arg(manufacturerId, batteryInsertationDate,
                                       extendedTransponderId);

    // Вычленяем символ F из personal_account_number
    QString tempPan = records.get(i, "personal_account_number");
    QString pan = tempPan.remove(QChar('F'));

    QString registerRecord =
        QString("%1;%2;%3;%4;%5\n")
            .arg(tempModel, sn, pan, records.get(i, "box_id"),
                 records.get(i, "pallet_id"));

    registerOut << registerRecord;
  }

  return ReturnStatus::NoError;
}
