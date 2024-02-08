#include "administration_system.h"
#include "definitions.h"
#include "global_environment.h"
#include "log_system.h"
#include "postgre_sql_database.h"
#include "sql_query_values.h"

AdministrationSystem::AdministrationSystem(const QString& name)
    : QObject(nullptr) {
  setObjectName(name);
  loadSettings();

  createDatabase();

  connect(this, &AdministrationSystem::logging,
          dynamic_cast<LogSystem*>(
              GlobalEnvironment::instance()->getObject("LogSystem")),
          &LogSystem::generate);
}

void AdministrationSystem::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
  Database->applySettings();
}

ReturnStatus AdministrationSystem::connectDatabase() {
  sendLog("Подключение к базе данных. ");
  if (!Database->connect()) {
    sendLog("Не удалось установить соединение с базой данных. ");
    return ReturnStatus::DatabaseConnectionError;
  }

  Database->setRecordMaxCount(10000);
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::disconnectDatabase() {
  sendLog("Отключение от базы данных. ");
  Database->disconnect();

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::getTable(const QString& tableName,
                                            SqlQueryValues& response) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);
  if (!Database->readRecords(tableName, response)) {
    sendLog(QString("Получена ошибка при получении данных из таблицы %1. ")
                .arg(tableName));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Данные из таблицы %1 успешно получены. ").arg(tableName));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::getCustomResponse(const QString& req,
                                                     SqlQueryValues& response) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!Database->execCustomRequest(req, response)) {
    sendLog("Получена ошибка при выполнении кастомного запроса. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog("Кастомный запрос успешно выполнен. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::createNewOrder(
    const StringDictionary& param) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog("Добавление заказа. ");
  if (!addOrder(param)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog("Новый заказ успешно создан. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::startOrderAssembling(const QString& id) {
  SqlQueryValues productionLines;
  SqlQueryValues orders;
  SqlQueryValues orderNewValue;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  // Проверяем, что в процессе сборки нет других заказов
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("orders", "in_process = true", orders)) {
    sendLog(QString(
        "Получена ошибка при поиске заказов, находящихся в процессе сборки."));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // В сборке может находится только один заказ
  if (!orders.isEmpty()) {
    sendLog(
        QString("В процессе сборки находится другой заказ, запуск нового "
                "заказа невозможен."));
    Database->rollbackTransaction();
    return ReturnStatus::OtherOrderInProcess;
  }

  orderNewValue.add("in_process", "true");
  orderNewValue.add("assembling_start", QDateTime::currentDateTime().toString(
                                            POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("orders", "id = " + id, orderNewValue)) {
    sendLog(
        QString("Получена ошибка при обновлении данных заказа %1.").arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Сборка заказа %1 запущена. ").arg(id));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::stopOrderAssembling(const QString& id) {
  SqlQueryValues orderNewValue;
  ReturnStatus status;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!stopAllProductionLines()) {
    sendLog("Получена ошибка при отключении всех производственных линий. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  orderNewValue.add("in_process", "false");
  if (!Database->updateRecords("orders", QString("id = %1").arg(id),
                               orderNewValue)) {
    sendLog(
        QString("Получена ошибка при обновлении данных заказа %1.").arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::createNewProductionLine(
    const StringDictionary& param) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  // Добавляем линию производства
  sendLog("Создание новой линии производства. ");
  if (!addProductionLine(param)) {
    sendLog("Получена ошибка при добавлении производственной линии. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog("Новая линия производства успешно создана. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::activateAllProductionLines() {
  SqlQueryValues productionLineNewValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog("Включение всех производственных линий. ");

  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines", productionLineNewValues)) {
    sendLog(QString(
        "Получена ошибка при обновлении данных в таблице production_lines."));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog(QString("Все линии производства остановлены. "));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::activateProductionLine(const QString& id) {
  SqlQueryValues productionLineNewValues;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  // Ищем данные заказа
  productionLineNewValues.add("active", "true");
  if (!Database->updateRecords("production_lines", QString("id = %1").arg(id),
                               productionLineNewValues)) {
    sendLog(QString(
        "Получена ошибка при изменении данных в таблице production_lines. "));
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Все линии производства запущены. "));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::deactivateProductionLine(const QString& id) {
  SqlQueryValues productionLineNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);

  // Останавливаем производственную линию
  productionLineNewValue.add("in_process", "false");
  productionLineNewValue.add("launched", "false");
  productionLineNewValue.add("completed", "false");
  productionLineNewValue.add("active", "false");
  productionLineNewValue.add("box_id", "NULL");
  productionLineNewValue.add("transponder_id", "NULL");
  if (!Database->updateRecords("production_lines", "id = " + id,
                               productionLineNewValue)) {
    sendLog(
        QString("Получена ошибка при обновлении данных линии производства."));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Линия производства %1 остановлена. ").arg(id));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::deactivateAllProductionLines() {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog("Отключение всех производственных линий. ");
  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Все линии производства остановлены. "));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::deleteLastProductionLine() {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  //  if (!removeLastProductionLine()) {
  //    sendLog("Получена ошибка при удалении последней линии производства. ");
  //    Database->rollbackTransaction();
  //    return ReturnStatus::DatabaseQueryError;
  //  }

  sendLog("Последняя линия производства успешно удалена. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::initIssuerTable() {
  SqlQueryValues newValues;
  int32_t id = 0;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog("Инициализация таблицы эмитентов. ");
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
    sendLog(
        QString("Получена при создании новых записей в таблице заказчиков. "));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog("Таблица эмитентов успешно инициализирована. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::initTransportMasterKeysTable() {
  SqlQueryValues transportKeys;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
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

  // Если транспортные ключи для эмитента отсутствуют, то создаем новую запись
  if (!Database->createRecords("transport_master_keys", transportKeys)) {
    sendLog(
        QString("Получена ошибка при добавлении транспортных мастер ключей. "));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog("Таблица транспортных ключей успешно инициализирована. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::linkIssuerWithMasterKeys(
    const StringDictionary& param) {
  SqlQueryValues issuerNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  sendLog(
      QString("Обновление записи эмитента %1.").arg(param.value("issuer_id")));
  if (param.value("key_table") == "transport_master_keys") {
    issuerNewValue.add("transport_master_keys_id", param.value("key_group_id"));
  } else {
    issuerNewValue.add("commercial_master_keys_id",
                       param.value("key_group_id"));
  }

  if (!Database->updateRecords("issuers", "id = " + param.value("issuer_id"),
                               issuerNewValue)) {
    sendLog(QString("Получена ошибка при обновлении записи эмитента %1.")
                .arg(param.value("issuer_id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  sendLog(QString("Эмитент %1 успешно связан с мастер ключами %2. ")
              .arg(param.value("issuer_id"), param.value("key_group_id")));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::getTransponderData(const QString& id,
                                                      StringDictionary& data) {
  QStringList tables{"transponders", "boxes", "pallets", "orders"};
  SqlQueryValues record;
  uint32_t quantity;

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(1);

  record.addField("transponder_model");
  record.addField("personal_account_number");
  record.addField("transponders.id");
  record.addField("box_id");
  record.addField("pallet_id");
  record.addField("boxes.assembling_start");
  record.addField("manufacturer_id");
  record.addField("issuers.name");
  if (!Database->readMergedRecords(tables, "transponders.id = " + id, record)) {
    sendLog(
        QString("Получена ошибка поиске транспондеров в паллете %1.").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  // Данные переносимые без изменений
  data.insert("box_id", record.get("box_id"));
  data.insert("pallet_id", record.get("pallet_id"));
  data.insert("order_id", record.get("order_id"));

  // Удаляем пробелы из названия модели
  QString tempModel = record.get("transponder_model");
  data.insert("transponder_model", tempModel.remove(" "));

  // Преобразуем в десятичный формат
  QString manufacturerId =
      QString::number(record.get("manufacturer_id").toInt(nullptr, 16));

  // Дата сборки
  QStringList tempDate = record.get("boxes.assembling_start").split("T");
  QDate date = QDate::fromString(tempDate.first(), "yyyy-MM-dd");
  QString batteryInsertationDate =
      QString("%1%2")
          .arg(QString::number(date.weekNumber()), 2, QChar('0'))
          .arg(QString::number(date.year() % 100), 2, QChar('0'));

  // Дополняем серийник до 10 цифр нулями слева
  QString extendedTransponderId =
      QString("%1").arg(record.get("transponders.id"), 10, QChar('0'));

  // Конструируем серийный номер транспондера
  data.insert("sn",
              QString("%1%2%3").arg(manufacturerId, batteryInsertationDate,
                                    extendedTransponderId));

  // Вычленяем символ F из personal_account_number
  QString tempPan = record.get("personal_account_number");
  data.insert("pan", tempPan.remove(QChar('F')));

  // Название компании-заказчика
  data.insert("issuer_name", record.get("issuers.name"));

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::getBoxData(const QString& id,
                                              StringDictionary& data) {
  SqlQueryValues box;
  SqlQueryValues transponders;
  StringDictionary transponderData;

  if (!Database->readRecords("boxes", "id + " + id, box)) {
    emit logging(QString("Получена ошибка при поиске бокса с id %1. ").arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Сохраняем данные бокса
  data.insert("id", id);
  data.insert("assembled_units", box.get("assembled_units"));

  // Ищем транспондеры в боксе
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(box.get("assembled_units").toUInt());
  if (!Database->readRecords("transponders", "box_id = " + id, transponders)) {
    emit logging(
        QString("Получена ошибка при поиске транспондеров в боксе %1. ")
            .arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (getTransponderData(transponders.get("id"), transponderData) !=
      ReturnStatus::NoError) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponders.get(0, "id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Сохраняем серийник первого транспондера в боксе
  data.insert("first_transponder_sn", transponderData.value("sn"));
  transponderData.clear();

  // Запрашиваем данные транспондера
  if (getTransponderData(transponders.getLast("id"), transponderData) !=
      ReturnStatus::NoError) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponders.get(box.get("assembled_units").toUInt() - 1,
                                  "id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Сохраняем серийник последнего транспондера в боксе
  data.insert("last_transponder_sn", transponderData.value("sn"));

  // Сохраняем модель транспондера
  QString modelTemp = transponderData.value("transponder_model");
  data.insert("transponder_model", modelTemp.remove(' '));

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::getPalletData(const QString& id,
                                                 StringDictionary& data) {
  SqlQueryValues boxes;
  SqlQueryValues pallet;
  SqlQueryValues order;

  if (!Database->readRecords("pallets", "id = " + id, pallet)) {
    emit logging(
        QString("Получена ошибка при поиске паллеты с id %1. ").arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->readRecords("orders", "id = " + pallet.get("order_id"),
                             order)) {
    emit logging(QString("Получена ошибка при поиске заказа с id %1. ")
                     .arg(pallet.get("order_id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Сохраняем данные паллеты
  data.insert("id", id);
  QStringList tempDate = pallet.get("assembling_end").split("T");
  data.insert(
      "assembly_date",
      QDate::fromString(tempDate.first(), "yyyy-MM-dd").toString("dd.MM.yyyy"));
  QString tempModel = order.get("transponder_model");
  data.insert("transponder_model", tempModel.remove(" "));

  // Ищем боксы в паллете
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(pallet.get("assembled_units").toUInt());
  if (!Database->readRecords("boxes", "pallet_id = " + id, boxes)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  // Сохраняем идентификатор первого бокса
  data.insert("first_box_id", boxes.get("id"));

  // Сохраняем идентификатор последнего бокса
  data.insert("last_box_id", boxes.getLast("id"));
  uint32_t totalQuantity =
      pallet.get("assembled_units").toInt() * boxes.get("quantity").toInt();

  data.insert("quantity", QString::number(totalQuantity));

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::rollbackProductionLine(const QString& id) {
  SqlQueryValues productionLine;
  SqlQueryValues productionLineNewValue;
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValue;
  SqlQueryValues box;
  SqlQueryValues boxNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  if (!Database->readRecords("production_lines", "id = " + id,
                             productionLine)) {
    sendLog(QString("Получена ошибка при поиске производственной линии '%1'. ")
                .arg(productionLine.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (productionLine.isEmpty()) {
    sendLog(
        QString("Производственная линия с идентификатором '%1' не найдена. ")
            .arg(id));
    Database->rollbackTransaction();
    return ReturnStatus::ProductionLineMissed;
  }

  if (!Database->readRecords("transponders",
                             "id = " + productionLine.get("box_id"),
                             transponder)) {
    sendLog(QString("Получена ошибка при поиске транспондера '%1'. ")
                .arg(productionLine.get("box_id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->readRecords("boxes", "id = " + transponder.get("box_id"),
                             box)) {
    sendLog(QString("Получена ошибка при поиске данных бокса '%1'. ")
                .arg(transponder.get("box_id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  Database->setCurrentOrder(Qt::DescendingOrder);
  Database->setRecordMaxCount(1);
  if (!Database->readRecords(
          "transponders", "release_counter >= 1 AND box_id = " + box.get("id"),
          transponder)) {
    sendLog(QString("Получена ошибка при поиске предыдущего транспондера "
                    "производственной линии в боксе %1. ")
                .arg(box.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);

  if (transponder.isEmpty()) {
    sendLog(QString("Производственная линия '%1' связана с первым "
                    "транспондером в боксе. Откат невозможен.")
                .arg(productionLine.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::ProductionLineRollbackLimit;
  }

  transponderNewValue.add("release_counter", "0");
  transponderNewValue.add("ucid", "NULL");
  transponderNewValue.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "id = " + transponder.get("id"),
                               transponderNewValue)) {
    sendLog(QString("Получена ошибка при возврате транспондера %1.")
                .arg(transponder.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  boxNewValue.add("assembled_units",
                  QString::number(box.get("assembled_units").toInt() - 1));
  if (!Database->updateRecords("boxes", "id = " + box.get("id"), boxNewValue)) {
    sendLog(QString("Получена ошибка при уменьшении количества собранных "
                    "транспондеров в боксе '%1'. ")
                .arg(box.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  productionLineNewValue.add("box_id", transponder.get("id"));
  if (!Database->updateRecords("production_lines", "id = " + id,
                               productionLineNewValue)) {
    sendLog(QString("Получена ошибка при связывании производственной линии %1 "
                    "с транспондером %2. ")
                .arg(id, transponder.get("id")));
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::releaseTransponder(const QString& id) {
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

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

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::releaseBox(const QString& id) {
  SqlQueryValues box;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;
  SqlQueryValues palletNewValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);

  if (!Database->readRecords("boxes", "id = " + id, box)) {
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
  if (!Database->updateRecords("boxes", "id = " + id, boxNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::releasePallet(const QString& id) {
  SqlQueryValues pallet;
  SqlQueryValues palletNewValue;
  SqlQueryValues boxes;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);

  if (!Database->readRecords("pallets", "id = " + id, pallet)) {
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
  if (!Database->updateRecords("pallets", "id = " + id, palletNewValue)) {
    sendLog(
        QString("Получена ошибка при изменении данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }
  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::releaseOrder(const QString& id) {
  SqlQueryValues order;
  SqlQueryValues orderNewValues;
  SqlQueryValues pallets;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);
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
  if (!Database->updateRecords("orders", "id = " + id, orderNewValues)) {
    sendLog(
        QString("Получена ошибка при изменении данных заказа %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::refundTransponder(const QString& id) {
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

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

  transponderNewValues.add("release_counter", "0");
  transponderNewValues.add("ucid", "NULL");
  transponderNewValues.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "id = " + id,
                               transponderNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных транспондера %1. ")
                .arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::refundBox(const QString& id) {
  SqlQueryValues box;
  SqlQueryValues transponderNewValues;
  SqlQueryValues boxNewValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);
  if (!Database->readRecords("boxes", "id = " + id, box)) {
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
  if (!Database->updateRecords("boxes", "id = " + id, boxNewValues)) {
    sendLog(QString("Получена ошибка при изменении данных бокса %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::refundPallet(const QString& id) {
  SqlQueryValues pallet;
  SqlQueryValues palletNewValue;
  SqlQueryValues boxes;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);
  if (!Database->readRecords("pallets", "id = " + id, pallet)) {
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
  if (!Database->updateRecords("pallets", "id = " + id, palletNewValue)) {
    sendLog(
        QString("Получена ошибка при изменении данных паллеты %1. ").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::refundOrder(const QString& id) {
  SqlQueryValues order;
  SqlQueryValues orderNewValues;
  SqlQueryValues pallets;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  Database->setRecordMaxCount(1);

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

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AdministrationSystem::shipPallets(const StringDictionary& param) {
  ReturnStatus ret;
  QString ShipmentRegisterName =
      QString("sr_pallets_%1_%2.csv")
          .arg(param.value("first_pallet_id"), param.value("last_pallet_id"));
  QFile file(ShipmentRegisterName);
  QTextStream out(&file);

  // Открываем файл для записи
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии файла реестра отгрузки. ");
    return ReturnStatus::RegisterFileError;
  }
  file.resize(0);
  out << "model;sn;pan;box_id;pallet_id\n";

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return ReturnStatus::DatabaseTransactionError;
  }

  for (uint32_t i = param.value("first_pallet_id").toUInt();
       i <= param.value("last_pallet_id").toUInt(); i++) {
    sendLog(QString("Отгрузка паллеты %1.").arg(QString::number(i)));
    ret = shipPallet(QString::number(i), out);
    if (ret != ReturnStatus::NoError) {
      sendLog(QString("Получена ошибка при отгрузке паллеты %1.")
                  .arg(QString::number(i)));
      file.close();
      Database->rollbackTransaction();
      return ReturnStatus::DatabaseQueryError;
    }
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return ReturnStatus::DatabaseTransactionError;
  }

  return ReturnStatus::NoError;
}

void AdministrationSystem::loadSettings() {
  QSettings settings;

  ShipmentRegisterDir = "/ShipmentRegisters/";
}

void AdministrationSystem::sendLog(const QString& log) {
  emit logging(QString("%1 - %2").arg(objectName(), log));
}

bool AdministrationSystem::addOrder(const StringDictionary& param) {
  uint32_t transponderCount = param.value("transponder_quantity").toInt();
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  SqlQueryValues issuerRecord;
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

  // Получаем последний идентификатор заказа
  lastId = getLastId("orders");

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

bool AdministrationSystem::addPallets(const QString& orderId,
                                      const StringDictionary& param) {
  uint32_t transponderCount = param.value("transponder_quantity").toInt();
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  SqlQueryValues newPalletValues;
  int32_t lastId = 0;

  // Получаем идентификатор последней палеты
  lastId = getLastId("pallets");

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

bool AdministrationSystem::addBoxes(const QString& palletId,
                                    const StringDictionary& param,
                                    QTextStream& panSource) {
  uint32_t palletCapacity = param.value("pallet_capacity").toInt();
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  SqlQueryValues newBoxValues;
  int32_t lastId = 0;

  // Получаем идентификатор последнего добавленного бокса
  lastId = getLastId("boxes");

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

bool AdministrationSystem::addTransponders(
    const QString& boxId,
    const std::shared_ptr<QVector<QString>>& pans,
    const StringDictionary& param) {
  uint32_t boxCapacity = param.value("box_capacity").toInt();
  int32_t lastId = 0;
  SqlQueryValues newTransponders;

  // Получаем идентификатор незаполненного бокса
  lastId = getLastId("transponders");

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

bool AdministrationSystem::addProductionLine(const StringDictionary& param) {
  SqlQueryValues newProductionLine;
  int32_t lastId = 0;

  // Получаем идентификатор последней линии производства
  lastId = getLastId("production_lines");

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

int32_t AdministrationSystem::getLastId(const QString& table) {
  SqlQueryValues record;

  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::DescendingOrder);
  if (!Database->readRecords(table, record)) {
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
  } else {
    return record.get("id").toInt();
  }
  Database->setRecordMaxCount(0);
  Database->setCurrentOrder(Qt::AscendingOrder);
}

bool AdministrationSystem::stopAllProductionLines() {
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

bool AdministrationSystem::startOrderProcessing(const QString& id) {
  SqlQueryValues orderRecord;

  orderRecord.add("in_process", "true");
  if (!Database->updateRecords("orders", "id = " + id, orderRecord)) {
    sendLog("Получена ошибка при запуске сборки заказа. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::stopOrderProcessing(const QString& id) {
  SqlQueryValues order;
  SqlQueryValues orderNewValues;

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(1);

  if (!Database->readRecords("orders", "id = " + id, order)) {
    sendLog(QString("Получена ошибка при поиске данных заказа %1. ").arg(id));
    return false;
  }

  orderNewValues.add("in_process", "false");
  if (!Database->updateRecords("pallets", "id = " + id, orderNewValues)) {
    sendLog(
        QString("Получена ошибка при остановке сборки заказа %1. ").arg(id));
    return false;
  }

  return true;
}

ReturnStatus AdministrationSystem::shipPallet(const QString& id,
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
  if (!Database->readMergedRecords(tables, "pallets.id = " + id, records)) {
    sendLog(
        QString("Получена ошибка поиске транспондеров в паллете %1.").arg(id));
    return ReturnStatus::DatabaseQueryError;
  }

  for (int32_t i = 0; i < records.recordCount(); i++) {
    // Удаляем пробелы из названия модели
    QString tempModel = records.get(i, "transponder_model");

    // Преобразуем в десятичный формат
    QString manufacturerId =
        QString::number(records.get(i, "manufacturer_id").toInt(nullptr, 16));

    // Дата сборки
    QStringList tempDate = records.get(i, "assembling_start").split("T");
    QDate date = QDate::fromString(tempDate.first(), POSTGRES_DATE_TEMPLATE);
    QString batteryInsertationDate =
        QString("%1%2")
            .arg(QString::number(date.weekNumber()), 2, QChar('0'))
            .arg(QString::number(date.year() % 100), 2, QChar('0'));

    // Дополняем серийник до 10 цифр нулями слева
    QString extendedTransponderId =
        QString("%1").arg(records.get(i, "transponders.id"), 10, QChar('0'));

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

void AdministrationSystem::createDatabase() {
  Database = std::unique_ptr<AbstractSqlDatabase>(
      new PostgreSqlDatabase("PostgreSqlDatabase", "AdministratorConnection"));
}
