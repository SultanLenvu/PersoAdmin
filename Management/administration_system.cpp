#include "administration_system.h"
#include "Database/sql_query_values.h"
#include "General/definitions.h"
#include "Log/log_system.h"

AdministrationSystem::AdministrationSystem(QObject* parent) : QObject(parent) {
  setObjectName("AdministrationSystem");
  loadSettings();

  createDatabase();
}

void AdministrationSystem::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
  Database->applySettings();
}

AdministrationSystem::ReturnStatus AdministrationSystem::connectDatabase() {
  sendLog("Подключение к базе данных. ");
  if (!Database->connect()) {
    sendLog("Не удалось установить соединение с базой данных. ");
    return DatabaseConnectionError;
  }

  Database->setRecordMaxCount(10000);
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::disconnectDatabase() {
  sendLog("Отключение от базы данных. ");
  Database->disconnect();

  return Completed;
}

void AdministrationSystem::createDatabase() {
  Database = new PostgreSqlDatabase(this, "AdministratorConnection");
  connect(Database, &AbstractSqlDatabase::logging, LogSystem::instance(),
          &LogSystem::generate);
}

AdministrationSystem::ReturnStatus AdministrationSystem::clearDatabaseTable(
    const QString& tableName) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->clearTable(tableName)) {
    sendLog(QString("Получена ошибка при очистке таблицы %1. ").arg(tableName));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Таблица %1 очищена. ").arg(tableName));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getDatabaseTable(
    const QString& tableName,
    SqlQueryValues* response) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);
  if (!Database->readRecords(tableName, *response)) {
    sendLog(QString("Получена ошибка при получении данных из таблицы %1. ")
                .arg(tableName));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Данные из таблицы %1 успешно получены. ").arg(tableName));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getCustomResponse(
    const QString& req,
    SqlQueryValues* response) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->execCustomRequest(req, *response)) {
    sendLog("Получена ошибка при выполнении кастомного запроса. ");
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog("Кастомный запрос успешно выполнен. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::createNewOrder(
    const QSharedPointer<QHash<QString, QString>> orderParameters) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Добавление заказа. ");
  if (!addOrder(orderParameters)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog("Новый заказ успешно создан. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::startOrderAssembling(
    const QString& id) {
  SqlQueryValues productionLines;
  SqlQueryValues orders;
  SqlQueryValues orderNewValue;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  // Проверяем, что в процессе сборки нет других заказов
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("orders", "in_process = true", orders)) {
    sendLog(QString(
        "Получена ошибка при поиске заказов, находящихся в процессе сборки."));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // В сборке может находится только один заказ
  if (!orders.isEmpty()) {
    sendLog(
        QString("В процессе сборки находится другой заказ, запуск нового "
                "заказа невозможен."));
    Database->rollbackTransaction();
    return OtherOrderInProcess;
  }

  orderNewValue.add("in_process", "true");
  if (!Database->updateRecords("orders", "id = " + id, orderNewValue)) {
    sendLog(
        QString("Получена ошибка при обновлении данных заказа %1.").arg(id));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Ищем свободные производственные линии
  Database->setRecordMaxCount(0);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("production_lines", "in_process = false",
                             productionLines)) {
    sendLog(
        QString("Получена ошибка при поиске незадействованной производственной "
                "линии."));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Если линии не найдены
  if (productionLines.isEmpty()) {
    sendLog(QString(
        "Не найдено ни одной незадействованной производственной линии. "));
    Database->rollbackTransaction();
    return ProductionLineMissed;
  }

  // Запускаем производственные линии
  for (int32_t i = 0; i < productionLines.recordCount(); i++) {
    ret = startProductionLine(productionLines.get(i, "id"), id);
    if (ret != Completed) {
      Database->rollbackTransaction();
      sendLog(QString("Получена ошибка при запуске производственной линии %1. ")
                  .arg(productionLines.get(i, "id")));
      return ret;
    }
  }

  sendLog(QString("Сборка заказа %1 запущена. ").arg(id));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::stopOrderAssembling(
    const QString& orderId) {
  ReturnStatus status;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Остановка всех производственных линий. ");
  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    sendLog("Получена ошибка при остановке всех производственных линий. ");
    return DatabaseQueryError;
  }

  sendLog(QString("Сборка заказа %1 остановлена. ").arg(orderId));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::deleteLastOrder() {
  SqlQueryValues lastOrder;

  sendLog("Удаление последнего добавленного заказа. ");

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->readLastRecord("orders", lastOrder)) {
    sendLog("Получена ошибка при открытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  if ((lastOrder.get("in_process") == "true") ||
      (lastOrder.get("quantity") == lastOrder.get("assembled_units"))) {
    sendLog(QString("Заказ %1 находится в процессе сборки, либо уже собран, "
                    "удаление невозможно. ")
                .arg(lastOrder.get("id")));
    Database->rollbackTransaction();
    return OrderRemovingError;
  }

  Database->deleteRecords("orders", "id = " + lastOrder.get("id"));

  sendLog("Последний добавленный заказ успешно удален. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::createNewProductionLine(
    const QHash<QString, QString>* productionLineParameters) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  // Добавляем линию производства
  sendLog("Создание новой линии производства. ");
  if (!addProductionLine(productionLineParameters)) {
    sendLog("Получена ошибка при добавлении производственной линии. ");
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog("Новая линия производства успешно создана. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::deleteLastProductionLine() {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  //  if (!removeLastProductionLine()) {
  //    sendLog("Получена ошибка при удалении последней линии производства. ");
  //    Database->rollbackTransaction();
  //    return DatabaseQueryError;
  //  }

  sendLog("Последняя линия производства успешно удалена. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::stopAllProductionLinesManually() const {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Остановка всех линий производства. ");
  if (!stopAllProductionLines()) {
    Database->rollbackTransaction();
    sendLog("Получена ошибка при остановке всех линий производства. ");
    return DatabaseQueryError;
  }

  sendLog(QString("Все линии производства остановлены. "));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::startProductionLineManually(const QString& id,
                                                  const QString& orderId) {
  SqlQueryValues order;
  ReturnStatus ret;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  // Ищем данные заказа
  if (!Database->readRecords("orders", "id = " + orderId, order)) {
    sendLog(QString("Получена ошибка при поиске активного заказа. "));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Запустить производственную линию можно только на активном заказе
  if (order.get("in_process") != "true") {
    sendLog(QString("Заказ %1 не находится в процессе сборки. Запуск "
                    "производственной линии %1 невозможен. ")
                .arg(orderId, id));
    Database->rollbackTransaction();
    return OrderNotInProcess;
  }

  ret = startProductionLine(id, orderId);
  if (ret != Completed) {
    sendLog(QString("Не удалось запустить линию производства %1. ").arg(id));
    Database->rollbackTransaction();
    return ret;
  }

  sendLog(QString("Все линии производства запущены. "));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::stopProductionLineManually(const QString& id) {
  SqlQueryValues productionLineNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  if (!getCurrentContext(id)) {
    sendLog(
        QString(
            "Получена ошибка при получении контекста линии производства %1. ")
            .arg(id));
    return DatabaseQueryError;
  }

  // Останавливаем производственную линию
  productionLineNewValue.add("in_process", "false");
  productionLineNewValue.add("box_id", "NULL");
  if (!Database->updateRecords("production_lines", "id = " + id,
                               productionLineNewValue)) {
    sendLog(
        QString("Получена ошибка при обновлении данных линии производства."));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Останавливаем сборку текущего бокса
  if (!stopCurrentBoxProcessing()) {
    sendLog(QString("Получена ошибка при остановке сборки бокса %1. ")
                .arg(CurrentBox.get("id")));
    return DatabaseQueryError;
  }

  sendLog(QString("Линия производства %1 остановлена. ").arg(id));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::initIssuerTable() {
  SqlQueryValues newValues;
  int32_t id = 0;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
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
    return DatabaseQueryError;
  }

  sendLog("Таблица эмитентов успешно инициализирована. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::initTransportMasterKeysTable() {
  SqlQueryValues transportKeys;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
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
    return DatabaseQueryError;
  }

  sendLog("Таблица транспортных ключей успешно инициализирована. ");
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::linkIssuerWithMasterKeys(
    const QHash<QString, QString>* linkParameters) {
  SqlQueryValues issuerNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }

  sendLog(QString("Обновление записи эмитента %1.")
              .arg(linkParameters->value("issuer_id")));
  if (linkParameters->value("master_keys_type") == "transport_master_keys") {
    issuerNewValue.add("transport_master_keys_id",
                       linkParameters->value("master_keys_id"));
  } else {
    issuerNewValue.add("commercial_master_keys_id",
                       linkParameters->value("master_keys_id"));
  }

  if (!Database->updateRecords("issuers",
                               "id = " + linkParameters->value("issuer_id"),
                               issuerNewValue)) {
    sendLog(QString("Получена ошибка при обновлении записи эмитента %1.")
                .arg(linkParameters->value("issuer_id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Эмитент %1 успешно связан с мастер ключами %2. ")
              .arg(linkParameters->value("issuer_id"),
                   linkParameters->value("master_keys_id")));
  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getTransponderData(
    const QString& id,
    QHash<QString, QString>* data) {
  if (!getCurrentContext(id)) {
    sendLog(QString("Получена ошибка при получении контекста транспондера %1. ")
                .arg(id));
    return DatabaseQueryError;
  }

  // Данные переносимые без изменений
  data->insert("box_id", CurrentBox.get("id"));
  data->insert("pallet_id", CurrentPallet.get("id"));
  data->insert("order_id", CurrentOrder.get("id"));

  // Удаляем пробелы из названия модели
  QString tempModel = CurrentOrder.get("transponder_model");
  data->insert("transponder_model", tempModel.remove(" "));

  // Преобразуем в десятичный формат
  QString manufacturerId =
      QString::number(CurrentOrder.get("manufacturer_id").toInt(nullptr, 16));

  // Дата сборки
  QStringList tempDate = CurrentBox.get("assembling_start").split("T");
  QDate date = QDate::fromString(tempDate.first(), "yyyy-MM-dd");
  QString batteryInsertationDate =
      QString("%1%2")
          .arg(QString::number(date.weekNumber()), 2, QChar('0'))
          .arg(QString::number(date.year() % 100), 2, QChar('0'));

  // Дополняем серийник до 10 цифр нулями слева
  QString extendedTransponderId =
      QString("%1").arg(CurrentTransponder.get("id"), 10, QChar('0'));

  // Конструируем серийный номер транспондера
  data->insert("sn",
               QString("%1%2%3").arg(manufacturerId, batteryInsertationDate,
                                     extendedTransponderId));

  // Вычленяем символ F из personal_account_number
  QString tempPan = CurrentTransponder.get("personal_account_number");
  data->insert("pan", tempPan.remove(QChar('F')));

  // Название компании-заказчика
  data->insert("issuer_name", CurrentIssuer.get("name"));

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getBoxData(
    const QString& id,
    QHash<QString, QString>* data) {
  SqlQueryValues box;
  SqlQueryValues transponders;
  QHash<QString, QString> transponderData;

  if (!Database->readRecords("boxes", "id + " + id, box)) {
    emit logging(QString("Получена ошибка при поиске бокса с id %1. ").arg(id));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем данные бокса
  data->insert("id", id);
  data->insert("assembled_units", box.get("assembled_units"));

  // Ищем транспондеры в боксе
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(box.get("assembled_units").toUInt());
  if (!Database->readRecords("transponders", "box_id = " + id, transponders)) {
    emit logging(
        QString("Получена ошибка при поиске транспондеров в боксе %1. ")
            .arg(id));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponders.get("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponders.get(0, "id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем серийник первого транспондера в боксе
  data->insert("first_transponder_sn", transponderData.value("sn"));
  transponderData.clear();

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponders.getLast("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponders.get(box.get("assembled_units").toUInt() - 1,
                                  "id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем серийник последнего транспондера в боксе
  data->insert("last_transponder_sn", transponderData.value("sn"));

  // Сохраняем модель транспондера
  QString modelTemp = transponderData.value("transponder_model");
  data->insert("transponder_model", modelTemp.remove(' '));

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getPalletData(
    const QString& id,
    QHash<QString, QString>* data) {
  SqlQueryValues boxes;
  SqlQueryValues pallet;
  SqlQueryValues order;

  if (!Database->readRecords("pallets", "id = " + id, pallet)) {
    emit logging(
        QString("Получена ошибка при поиске паллеты с id %1. ").arg(id));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  if (!Database->readRecords("orders", "id = " + pallet.get("order_id"),
                             order)) {
    emit logging(QString("Получена ошибка при поиске заказа с id %1. ")
                     .arg(pallet.get("order_id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем данные паллеты
  data->insert("id", id);
  QStringList tempDate = pallet.get("assembling_end").split("T");
  data->insert(
      "assembly_date",
      QDate::fromString(tempDate.first(), "yyyy-MM-dd").toString("dd.MM.yyyy"));
  QString tempModel = order.get("transponder_model");
  data->insert("transponder_model", tempModel.remove(" "));

  // Ищем боксы в паллете
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(pallet.get("assembled_units").toUInt());
  if (!Database->readRecords("boxes", "pallet_id = " + id, boxes)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем идентификатор первого бокса
  data->insert("first_box_id", boxes.get("id"));

  // Сохраняем идентификатор последнего бокса
  data->insert("last_box_id", boxes.getLast("id"));
  uint32_t totalQuantity =
      pallet.get("assembled_units").toInt() * boxes.get("quantity").toInt();

  data->insert("quantity", QString::number(totalQuantity));

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::rollbackProductionLine(
    const QString& id) {
  SqlQueryValues productionLine;
  SqlQueryValues productionLineNewValue;
  SqlQueryValues transponder;
  SqlQueryValues transponderNewValue;
  SqlQueryValues box;
  SqlQueryValues boxNewValue;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->readRecords("production_lines", "id = " + id,
                             productionLine)) {
    sendLog(QString("Получена ошибка при поиске производственной линии '%1'. ")
                .arg(productionLine.get("id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  if (productionLine.isEmpty()) {
    sendLog(
        QString("Производственная линия с идентификатором '%1' не найдена. ")
            .arg(id));
    Database->rollbackTransaction();
    return ProductionLineMissed;
  }

  if (!Database->readRecords("transponders",
                             "id = " + productionLine.get("box_id"),
                             transponder)) {
    sendLog(QString("Получена ошибка при поиске транспондера '%1'. ")
                .arg(productionLine.get("box_id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  if (!Database->readRecords("boxes", "id = " + transponder.get("box_id"),
                             box)) {
    sendLog(QString("Получена ошибка при поиске данных бокса '%1'. ")
                .arg(transponder.get("box_id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
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
    return DatabaseQueryError;
  }
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);

  if (transponder.isEmpty()) {
    sendLog(QString("Производственная линия '%1' связана с первым "
                    "транспондером в боксе. Откат невозможен.")
                .arg(productionLine.get("id")));
    Database->rollbackTransaction();
    return ProductionLineRollbackLimit;
  }

  transponderNewValue.add("release_counter", "0");
  transponderNewValue.add("ucid", "NULL");
  transponderNewValue.add("awaiting_confirmation", "false");
  if (!Database->updateRecords("transponders", "id = " + transponder.get("id"),
                               transponderNewValue)) {
    sendLog(QString("Получена ошибка при возврате транспондера %1.")
                .arg(transponder.get("id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  boxNewValue.add("assembled_units",
                  QString::number(box.get("assembled_units").toInt() - 1));
  if (!Database->updateRecords("boxes", "id = " + box.get("id"), boxNewValue)) {
    sendLog(QString("Получена ошибка при уменьшении количества собранных "
                    "транспондеров в боксе '%1'. ")
                .arg(box.get("id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  productionLineNewValue.add("box_id", transponder.get("id"));
  if (!Database->updateRecords("production_lines", "id = " + id,
                               productionLineNewValue)) {
    sendLog(QString("Получена ошибка при связывании производственной линии %1 "
                    "с транспондером %2. ")
                .arg(id, transponder.get("id")));
    Database->rollbackTransaction();
    return DatabaseQueryError;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::releaseTranspondersManually(const QString& table,
                                                  const QString& id) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  ReturnStatus ret;
  if (table == "transponders") {
    ret = releaseTransponderManually(id);
  } else if (table == "boxes") {
    ret = releaseBoxManually(id);
  } else if (table == "pallets") {
    ret = releasePalletManually(id);
  } else if (table == "orders") {
    ret = releaseOrderManually(id);
  } else {
    sendLog("Получено неизвестное имя таблицы. ");
    Database->rollbackTransaction();
    return ParameterError;
  }

  if (ret != Completed) {
    Database->rollbackTransaction();
    return ret;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::refundTranspondersManually(const QString& table,
                                                 const QString& id) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  ReturnStatus ret;
  if (table == "transponders") {
    ret = refundTransponderManually(id);
  } else if (table == "boxes") {
    ret = refundBoxManually(id);
  } else if (table == "pallets") {
    ret = refundPalletManually(id);
  } else if (table == "orders") {
    ret = refundOrderManually(id);
  } else {
    sendLog("Получено неизвестное имя таблицы. ");
    Database->rollbackTransaction();
    return ParameterError;
  }

  if (ret != Completed) {
    Database->rollbackTransaction();
    return ret;
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::shipPallets(
    const QHash<QString, QString>* param) {
  ReturnStatus ret;
  QString ShipmentRegisterName =
      QString("sr_pallets_%1_%2.csv")
          .arg(param->value("first_pallet_id"), param->value("last_pallet_id"));
  QFile file(ShipmentRegisterName);
  QTextStream out(&file);

  // Открываем файл для записи
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии файла реестра отгрузки. ");
    return RegisterFileError;
  }
  file.resize(0);
  out << "model;sn;pan;box_id;pallet_id\n";

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытии транзакции. ");
    return DatabaseTransactionError;
  }

  for (uint32_t i = param->value("first_pallet_id").toUInt();
       i <= param->value("last_pallet_id").toUInt(); i++) {
    sendLog(QString("Отгрузка паллеты %1.").arg(QString::number(i)));
    ret = shipPallet(QString::number(i), out);
    if (ret != Completed) {
      sendLog(QString("Получена ошибка при отгрузке паллеты %1.")
                  .arg(QString::number(i)));
      file.close();
      Database->rollbackTransaction();
      return DatabaseQueryError;
    }
  }

  if (!Database->commitTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->rollbackTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

void AdministrationSystem::loadSettings() {
  QSettings settings;

  ShipmentRegisterDir = "/ShipmentRegisters/";
  LogEnable = settings.value("log_system/global_enable").toBool();
}

void AdministrationSystem::sendLog(const QString& log) const {
  if (LogEnable) {
    emit const_cast<AdministrationSystem*>(this)->logging(
        QString("%1 - %2").arg(objectName(), log));
  }
}

bool AdministrationSystem::getCurrentContext(const QString& id) {
  if (!Database->readRecords("transponders", QString("id = %1").arg(id),
                             CurrentTransponder)) {
    sendLog(QString("Получена ошибка при получении данных транспондера %1. ")
                .arg(id));
    return false;
  }

  if (!Database->readRecords(
          "boxes", QString("id = %1").arg(CurrentTransponder.get("box_id")),
          CurrentBox)) {
    sendLog(QString("Получена ошибка при получении данных бокса %1. ")
                .arg(CurrentTransponder.get("box_id")));
    return false;
  }

  if (!Database->readRecords(
          "pallets", QString("id = %1").arg(CurrentBox.get("pallet_id")),
          CurrentPallet)) {
    sendLog(QString("Получена ошибка при получении данных паллеты %1. ")
                .arg(CurrentBox.get("pallet_id")));
    return false;
  }

  if (!Database->readRecords(
          "orders", QString("id = %1").arg(CurrentPallet.get("order_id")),
          CurrentOrder)) {
    sendLog(QString("Получена ошибка при получении данных заказа %1. ")
                .arg(CurrentPallet.get("order_id")));
    return false;
  }

  if (!Database->readRecords(
          "issuers", QString("id = %1").arg(CurrentOrder.get("issuer_id")),
          CurrentIssuer)) {
    sendLog(QString("Получена ошибка при получении данных заказчика %1. ")
                .arg(CurrentOrder.get("issuer_id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::addOrder(
    const QSharedPointer<QHash<QString, QString>> orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  SqlQueryValues issuerRecord;
  SqlQueryValues orderRecord;
  SqlQueryValues newOrderValues;
  int32_t lastId = 0;

  // Ищем заказчика
  if (!Database->readRecords(
          "issuers",
          QString("name = '%1'").arg(orderParameters->value("issuer_name")),
          issuerRecord)) {
    sendLog(QString("Получена ошибка при поиске эмитента \"%1\".")
                .arg(orderParameters->value("issuer_name")));
    return false;
  }

  // Получаем последний идентификатор заказа
  lastId = getLastId("orders");

  // Формируем новую запись
  newOrderValues.add("id", QString::number(lastId + 1));
  newOrderValues.add("issuer_id", issuerRecord.get("id"));
  newOrderValues.add("quantity", QString::number(orderCapacity));
  newOrderValues.add("full_personalization",
                     orderParameters->value("full_personalization"));
  newOrderValues.add(
      "transponder_model",
      orderParameters->value("transponder_model")
          .rightJustified(TRANSPONDER_MODEL_CHAR_LENGTH, QChar(' ')));
  newOrderValues.add("accr_reference",
                     orderParameters->value("accr_reference"));
  newOrderValues.add("equipment_class",
                     orderParameters->value("equipment_class"));
  newOrderValues.add("manufacturer_id",
                     orderParameters->value("manufacturer_id"));
  if (!Database->createRecords("orders", newOrderValues)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    return false;
  }

  // Добавляем паллеты в заказ
  return addPallets(newOrderValues.get("id"), orderParameters);
}

bool AdministrationSystem::addPallets(
    const QString& orderId,
    const QSharedPointer<QHash<QString, QString>> orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
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
  QFile panFile(orderParameters->value("pan_file_path"));
  if (!panFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии PAN-файла. ");
    return false;
  }
  QTextStream panSource(&panFile);

  // Добавляем боксы в паллеты
  for (uint32_t i = 0; i < orderCapacity; i++) {
    if (!addBoxes(newPalletValues.get(i, "id"), orderParameters, panSource)) {
      sendLog(QString("Получена ошибка при добавлении боксов в паллету %1. ")
                  .arg(newPalletValues.get(i, "id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::addBoxes(
    const QString& palletId,
    const QSharedPointer<QHash<QString, QString>> orderParameters,
    QTextStream& panSource) const {
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
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
    QSharedPointer<QVector<QString>> pans(new QVector<QString>());
    for (int32_t j = 0; j < boxCapacity && !panSource.atEnd(); j++) {
      pans->append(
          panSource.readLine().leftJustified(FULL_PAN_CHAR_LENGTH, QChar('F')));
    }
    if (!addTransponders(newBoxValues.get(i, "id"), pans, orderParameters)) {
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
    const QSharedPointer<QVector<QString>> pans,
    const QSharedPointer<QHash<QString, QString>> orderParameters) const {
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
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

bool AdministrationSystem::addProductionLine(
    const QHash<QString, QString>* productionLineParameters) const {
  SqlQueryValues newProductionLine;
  int32_t lastId = 0;

  // Получаем идентификатор последней линии производства
  lastId = getLastId("production_lines");

  // Создаем новую линию производства
  newProductionLine.add("login", productionLineParameters->value("login"));
  newProductionLine.add("password",
                        productionLineParameters->value("password"));
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

int32_t AdministrationSystem::getLastId(const QString& table) const {
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

AdministrationSystem::ReturnStatus AdministrationSystem::startProductionLine(
    const QString& id,
    const QString& orderId) {
  SqlQueryValues box;

  // Ищем свободный бокс в заказе
  if (!searchFreeBox(orderId, id, box)) {
    sendLog(QString("Получена ошибка при поиске свободного бокса в заказе %1. ")
                .arg(orderId));
    return DatabaseQueryError;
  }

  // Если свободных боксов не найдено
  if (box.isEmpty()) {
    sendLog(QString("В заказе %1 все боксы заняты. ").arg(orderId));
    return FreeBoxMissed;
  }

  // Запускаем сборку бокса
  if (!startBoxProcessing(box.get("id"))) {
    sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                .arg(box.get("id")));
    return DatabaseQueryError;
  }

  // Связываем производственную линию
  if (!linkProductionLineWithBox(id, box.get("id"))) {
    sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                .arg(box.get("id")));
    return DatabaseQueryError;
  }

  return Completed;
}

bool AdministrationSystem::stopAllProductionLines() const {
  SqlQueryValues productionLineNewValues;
  SqlQueryValues boxNewValues;
  SqlQueryValues palletNewValues;
  SqlQueryValues orderNewValues;

  productionLineNewValues.add("in_process", "false");
  productionLineNewValues.add("box_id", "NULL");
  if (!Database->updateRecords("production_lines", "in_process = true",
                               productionLineNewValues)) {
    sendLog(QString("Получена ошибка при остановке линий производства. "));
    return false;
  }

  sendLog("Остановка процесса сборки всех боксов. ");
  boxNewValues.add("in_process", "false");
  if (!Database->updateRecords("boxes", "in_process = true", boxNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех боксов. "));
    Database->rollbackTransaction();
    return false;
  }

  sendLog("Остановка процесса сборки всех паллет.");
  palletNewValues.add("in_process", "false");
  if (!Database->updateRecords("pallets", "in_process = true",
                               palletNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех паллет."));
    Database->rollbackTransaction();
    return false;
  }

  sendLog("Остановка процесса сборки всех заказов.");
  orderNewValues.add("in_process", "false");
  if (!Database->updateRecords("orders", "in_process = true", orderNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех заказов."));
    Database->rollbackTransaction();
    return false;
  }
}

bool AdministrationSystem::linkProductionLineWithBox(
    const QString& id,
    const QString& boxId) const {
  SqlQueryValues boxNewValues;
  SqlQueryValues productionLineNewValues;

  // Связываем бокс с производственной линией
  boxNewValues.add("production_line_id", id);
  if (!Database->updateRecords("boxes", "id = " + boxId, boxNewValues)) {
    sendLog(QString("Получена ошибка при связывании бокса %1 с "
                    "производственной линией %2")
                .arg(boxId, id));
    return false;
  }

  // Связываем производственную линию c боксом
  productionLineNewValues.add("box_id", boxId);
  productionLineNewValues.add("in_process", "true");
  if (!Database->updateRecords("production_lines", "id = " + id,
                               productionLineNewValues)) {
    sendLog(
        QString("Получена ошибка при связывании производственной линии %1 с "
                "боксом %2. ")
            .arg(id, boxId));
    return false;
  }

  return true;
}

bool AdministrationSystem::startBoxProcessing(const QString& id) const {
  SqlQueryValues boxRecord;
  SqlQueryValues palletRecord;
  SqlQueryValues boxNewValues;

  // Получаем данные о боксе
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("boxes", "id = " + id, boxRecord)) {
    sendLog(QString("Получена ошибка при поиске данных бокса %1. ").arg(id));
    return false;
  }

  // Запускаем сборку бокса
  boxNewValues.add("in_process", "true");
  boxNewValues.add("assembling_start", QDateTime::currentDateTime().toString(
                                           POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("boxes", "id = " + id, boxNewValues)) {
    sendLog(QString("Получена ошибка при запуске сборки бокса %1. ").arg(id));
    return false;
  }

  if (!Database->readRecords("pallets", "id = " + boxRecord.get("pallet_id"),
                             palletRecord)) {
    sendLog(QString("Получена ошибка при поиске данных паллеты %1. ")
                .arg(boxRecord.get("pallet_id")));
    return false;
  }

  if (palletRecord.get("in_process") == "false") {
    // Запуск сборки палеты
    if (!startPalletProcessing(boxRecord.get("pallet_id"))) {
      sendLog(QString("Не удалось запустить сборку палеты %1. ")
                  .arg(boxRecord.get("pallet_id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::startPalletProcessing(const QString& id) const {
  SqlQueryValues palletRecord;
  SqlQueryValues orderRecord;
  SqlQueryValues palletNewValues;

  // Получаем данные о паллете
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  if (!Database->readRecords("pallets", "id = " + id, palletRecord)) {
    sendLog(QString("Получена ошибка при поиске данных паллеты %1. ").arg(id));
    return false;
  }

  palletNewValues.add("in_process", "true");
  palletNewValues.add("assembling_start", QDateTime::currentDateTime().toString(
                                              POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("pallets", "id = " + id, palletNewValues)) {
    sendLog("Получена ошибка при запуске сборки палеты. ");
    return false;
  }

  if (!Database->readRecords("orders", "id = " + palletRecord.get("order_id"),
                             orderRecord)) {
    sendLog(QString("Получена ошибка при поиске данных заказа %1. ")
                .arg(palletRecord.get("order_id")));
    return false;
  }

  if (orderRecord.get("in_process") == "false") {
    // Запуск сборки заказа
    if (!startOrderProcessing(palletRecord.get("order_id"))) {
      sendLog(QString("Не удалось запустить сборку заказа. ")
                  .arg(palletRecord.get("order_id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::startOrderProcessing(const QString& id) const {
  SqlQueryValues orderRecord;

  orderRecord.add("in_process", "true");
  orderRecord.add("assembling_start", QDateTime::currentDateTime().toString(
                                          POSTGRES_TIMESTAMP_TEMPLATE));
  if (!Database->updateRecords("orders", "id = " + id, orderRecord)) {
    sendLog("Получена ошибка при запуске сборки заказа. ");
    return false;
  }

  return true;
}

// bool AdministrationSystem::removeLastProductionLine() const {
//   SqlQueryValues productionLineRecord;
//   SqlQueryValues boxRecord;

//  // Получение последней добавленной линии производства
//  productionLineRecord.insert("id", "");
//  productionLineRecord.insert("active", "");
//  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
//    sendLog(
//        "Получена ошибка при поиске последней добавленной производственной "
//        "линии. ");
//    return false;
//  }

// Если производственная линия активна, то удаление невозможно
// if (productionLineRecord.value("active") == "true") {
//  sendLog("Производственная линия активна, удаление невозможно. ");
//  return false;
//}

//// Удаляем запись из таблицы линий производства
// if (!Database->removeLastRecordById("production_lines")) {
//   sendLog("Получена ошибка при удалении последней линии производства. ");
//   return false;
// }

// return true;
// }

bool AdministrationSystem::stopCurrentBoxProcessing() const {
  SqlQueryValues otherBoxes;
  SqlQueryValues currentBoxNewValues;

  // Если ни одного транспондера не было собрано в боксе, то его можно отвязать
  // от производственной линии
  if (CurrentBox.get("assembled_units") == "0") {
    currentBoxNewValues.add("production_line_id", "NULL");
  }
  currentBoxNewValues.add("in_process", "false");
  if (!Database->updateRecords("boxes", "id = " + CurrentBox.get("id"),
                               currentBoxNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки бокса %1. ")
                .arg(CurrentBox.get("id")));
    return false;
  }

  // Проверка наличия в текущей палете других боксов, находящихся в процессе
  // сборки
  if (!Database->readRecords("boxes", "pallet_id = " + CurrentPallet.get("id"),
                             otherBoxes)) {
    sendLog(
        QString("Получена ошибка при поиске боксов, содержащися в паллете %1. ")
            .arg(CurrentPallet.get("id")));
    return false;
  }

  // Если в паллете больше нет активных боксов, то останавливаем сборку паллеты
  if (otherBoxes.isEmpty()) {
    // Остановка сборки паллеты
    sendLog(
        QString("Остановка сборки паллеты %1. ").arg(CurrentPallet.get("id")));
    if (!stopCurrentPalletProcessing()) {
      sendLog(QString("Не удалось остановить сборку палеты. ")
                  .arg(CurrentPallet.get("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::stopCurrentPalletProcessing() const {
  SqlQueryValues otherPallets;
  SqlQueryValues palletNewValues;

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(1);

  palletNewValues.add("in_process", "false");
  if (!Database->updateRecords("pallets", "id = " + CurrentPallet.get("id"),
                               palletNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки паллеты %1. ")
                .arg(CurrentPallet.get("id")));
    return false;
  }

  // Проверка наличия в палете других боксов , находящихся в процессе сборки
  if (!Database->readRecords("pallets", "order_id = " + CurrentOrder.get("id"),
                             otherPallets)) {
    sendLog(
        QString("Получена ошибка при поиске паллет, содержащися в заказе %1. ")
            .arg(CurrentOrder.get("id")));
    return false;
  }

  // Если в паллете больше нет активных боксов, то останавливаем сборку паллеты
  if (otherPallets.isEmpty()) {
    // Остановка сборки паллеты
    sendLog(
        QString("Остановка сборки заказа %1. ").arg(CurrentOrder.get("id")));
    if (!stopCurrentOrderProcessing()) {
      sendLog(QString("Не удалось остановить сборку палеты. ")
                  .arg(CurrentOrder.get("id")));
      return false;
    }
  }

  Database->setRecordMaxCount(0);
  return true;
}

bool AdministrationSystem::stopCurrentOrderProcessing() const {
  SqlQueryValues orderNewValues;

  orderNewValues.add("in_process", "false");
  if (!Database->updateRecords("pallets", "id = " + CurrentOrder.get("id"),
                               orderNewValues)) {
    sendLog(QString("Получена ошибка при остановке сборки заказа %1. ")
                .arg(CurrentOrder.get("id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::searchFreeBox(const QString& orderId,
                                         const QString& productionLineId,
                                         SqlQueryValues& boxRecord) const {
  QStringList tables{"boxes", "pallets"};

  // Ищем незавершенные боксы
  Database->setRecordMaxCount(1);
  Database->setCurrentOrder(Qt::AscendingOrder);
  boxRecord.addField("boxes.id");
  if (!Database->readMergedRecords(
          tables,
          QString("boxes.assembled_units > 0 AND boxes.production_line_id = %1 "
                  "AND pallets.order_id = %2 AND boxes.in_process = false AND "
                  "boxes.assembled_units < boxes.quantity")
              .arg(productionLineId, orderId),
          boxRecord)) {
    sendLog(
        QString("Получена ошибка при поиске незавершенного бокса в заказе %1. ")
            .arg(orderId));
    return false;
  }

  // Если незавершенных боксов нет, то ищем свободные боксы
  if (boxRecord.isEmpty()) {
    boxRecord.clear();
    boxRecord.addField("boxes.id");
    if (!Database->readMergedRecords(
            tables,
            QString("boxes.assembled_units = 0 AND pallets.order_id = %2 AND "
                    "boxes.in_process = false")
                .arg(orderId),
            boxRecord)) {
      sendLog(
          QString("Получена ошибка при поиске свободного бокса в заказе %1. ")
              .arg(orderId));
      return false;
    }
  }

  return true;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::releaseTransponderManually(const QString& id) {
  //  if (!getCurrentContext(id)) {
  //    sendLog(QString("Получена ошибка при получении контекста транспондера
  //    %1. ")
  //                .arg(id));
  //    return DatabaseQueryError;
  //  }

  //  if (CurrentTransponder.value("release_counter").toInt() > 0) {
  //    sendLog(
  //        QString("Транспондер уже был выпущен ранее, принудительный выпуск "
  //                "невозможен. "));
  //    return LogicError;
  //  }

  //  CurrentTransponder.insert(
  //      "release_counter",
  //      QString::number(CurrentTransponder.value("release_counter").toInt() +
  //      1));
  //  CurrentTransponder.insert("ucid", "NULL");
  //  CurrentTransponder.insert("awaiting_confirmation", "false");
  //  if (!Database->updateRecords("transponders", CurrentTransponder)) {
  //    sendLog(
  //        QString("Получена ошибка при принудительном выпуске транспондера %1.
  //        ")
  //            .arg(id));
  //    return DatabaseQueryError;
  //  }

  //  CurrentBox.insert(
  //      "assembled_units",
  //      QString::number(CurrentBox.value("assembled_units").toInt() + 1));
  //  CurrentBox.insert("assembling_start", "NULL");
  //  CurrentBox.insert("assembling_end", "NULL");
  //  CurrentBox.insert("production_line_id", "NULL");
  //  if (!Database->updateRecords("boxes", CurrentBox)) {
  //    sendLog(QString("Получена ошибка при увеличении количества собранных "
  //                    "транспондеров в боксе %1. ")
  //                .arg(CurrentBox.value("id")));
  //    Database->rollbackTransaction();
  //    return DatabaseQueryError;
  //  }

  //  if (CurrentBox.value("assembled_units") == CurrentBox.value("quantity")) {
  //    CurrentBox.insert("ready_indicator", "true");
  //    if (!Database->updateRecords("boxes", CurrentBox)) {
  //      sendLog(QString("Получена ошибка при завершении сборки бокса %1. ")
  //                  .arg(CurrentBox.value("id")));
  //      return DatabaseQueryError;
  //    }

  //    CurrentPallet.insert(
  //        "assembled_units",
  //        QString::number(CurrentPallet.value("assembled_units").toInt() +
  //        1));
  //    CurrentPallet.insert("assembling_start", "NULL");
  //    CurrentPallet.insert("assembling_end", "NULL");
  //    if (!Database->updateRecords("pallets", CurrentPallet)) {
  //      sendLog(QString("Получена ошибка при увеличении количества собранных "
  //                      "боксов в паллете %1. ")
  //                  .arg(CurrentPallet.value("id")));
  //      return DatabaseQueryError;
  //    }

  //    if (CurrentPallet.value("assembled_units") ==
  //        CurrentPallet.value("quantity")) {
  //      CurrentPallet.insert("ready_indicator", "true");
  //      if (!Database->updateRecords("pallets", CurrentPallet)) {
  //        sendLog(QString("Получена ошибка при завершении сборки паллеты %1.
  //        ")
  //                    .arg(CurrentPallet.value("id")));
  //        return DatabaseQueryError;
  //      }

  //      CurrentOrder.insert(
  //          "assembled_units",
  //          QString::number(CurrentOrder.value("assembled_units").toInt() +
  //          1));
  //      CurrentOrder.insert("assembling_start", "NULL");
  //      CurrentOrder.insert("assembling_end", "NULL");
  //      if (!Database->updateRecords("orders", CurrentOrder)) {
  //        sendLog(QString("Получена ошибка при увеличении количества собранных
  //        "
  //                        "паллет в заказе %1. ")
  //                    .arg(CurrentOrder.value("id")));
  //        return DatabaseQueryError;
  //      }

  //      if (CurrentOrder.value("assembled_units") ==
  //          CurrentOrder.value("quantity")) {
  //        CurrentOrder.insert("ready_indicator", "true");
  //        if (!Database->updateRecords("orders", CurrentOrder)) {
  //          sendLog(QString("Получена ошибка при завершении сборки заказа %1.
  //          ")
  //                      .arg(CurrentOrder.value("id")));
  //          return DatabaseQueryError;
  //        }
  //      }
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releaseBoxManually(
    const QString& id) {
  //  QHash<QString, QString> transponderRecord;

  //  while (1) {
  //    transponderRecord.insert("id", "");
  //    transponderRecord.insert("box_id", id);
  //    transponderRecord.insert("release_counter", "0");
  //    if (!Database->getRecordByPart("transponders", transponderRecord)) {
  //      sendLog(QString("Получена ошибка при поиске транспондера в боксе %1.
  //      ")
  //                  .arg(transponderRecord.value("box_id")));
  //      return DatabaseQueryError;
  //    }

  //    if (transponderRecord.isEmpty()) {
  //      sendLog(
  //          QString("Все транспондеры из бокса %1 были принудительно выпущены.
  //          ")
  //              .arg(transponderRecord.value("box_id")));
  //      break;
  //    }

  //    ReturnStatus ret =
  //        releaseTransponderManually(transponderRecord.value("id"));
  //    if (ret != Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releasePalletManually(
    const QString& id) {
  //  QHash<QString, QString> boxRecord;

  //  while (1) {
  //    boxRecord.insert("id", "");
  //    boxRecord.insert("pallet_id", id);
  //    boxRecord.insert("ready_indicator", "false");
  //    if (!Database->getRecordByPart("boxes", boxRecord)) {
  //      sendLog(
  //          QString("Получена ошибка при поиске бокса в паллете %1.
  //          ").arg(id));
  //      return DatabaseQueryError;
  //    }

  //    if (boxRecord.isEmpty()) {
  //      sendLog(QString("Все боксы в паллете %1 были принудительно выпущены.
  //      ")
  //                  .arg(id));
  //      break;
  //    }

  //    ReturnStatus ret = releaseBoxManually(boxRecord.value("id"));
  //    if (ret != Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releaseOrderManually(
    const QString& id) {
  //  QHash<QString, QString> palletRecord;

  //  while (1) {
  //    palletRecord.insert("id", "");
  //    palletRecord.insert("order_id", id);
  //    palletRecord.insert("ready_indicator", "false");
  //    if (!Database->getRecordByPart("pallets", palletRecord)) {
  //      sendLog(
  //          QString("Получена ошибка при поиске паллеты в заказе %1.
  //          ").arg(id));
  //      return DatabaseQueryError;
  //    }

  //    if (palletRecord.isEmpty()) {
  //      sendLog(QString("Все паллеты в заказе %1 были принудительно выпущены.
  //      ")
  //                  .arg(palletRecord.value("order_id")));
  //      break;
  //    }

  //    ReturnStatus ret = releasePalletManually(palletRecord.value("id"));
  //    if (ret != Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::refundTransponderManually(const QString& id) {
  //  if (!getCurrentContext(id)) {
  //    sendLog(QString("Получена ошибка при получении контекста транспондера
  //    %1. ")
  //                .arg(id));
  //    return DatabaseQueryError;
  //  }

  //  CurrentTransponder.insert("release_counter", "0");
  //  CurrentTransponder.insert("ucid", "NULL");
  //  CurrentTransponder.insert("awaiting_confirmation", "false");
  //  if (!Database->updateRecords("transponders", CurrentTransponder)) {
  //    sendLog(QString("Получена ошибка при возврате транспондера %1.
  //    ").arg(id)); return DatabaseQueryError;
  //  }

  //  if ((CurrentPallet.value("ready_indicator") == "true") &&
  //      (CurrentOrder.value("assembled_units").toInt() > 0)) {
  //    if ((CurrentPallet.value("shipped_indicator") == "true") &&
  //        (CurrentOrder.value("shipped_units").toInt() > 0)) {
  //      CurrentOrder.insert(
  //          "shipped_units",
  //          QString::number(CurrentOrder.value("shipped_units").toInt() - 1));
  //    }
  //    CurrentOrder.insert(
  //        "assembled_units",
  //        QString::number(CurrentOrder.value("assembled_units").toInt() - 1));
  //    CurrentOrder.insert("ready_indicator", "false");
  //    CurrentOrder.insert("fully_shipped", "false");
  //    CurrentOrder.insert("assembling_start", "NULL");
  //    CurrentOrder.insert("assembling_end", "NULL");
  //    if (!Database->updateRecords("orders", CurrentOrder)) {
  //      sendLog(QString("Получена ошибка при снятии статуса готовности "
  //                      "заказа '%1'. ")
  //                  .arg(CurrentOrder.value("id")));
  //      return DatabaseQueryError;
  //    }
  //  }

  //  if ((CurrentBox.value("ready_indicator") == "true") &&
  //      (CurrentPallet.value("assembled_units").toInt() > 0)) {
  //    CurrentPallet.insert(
  //        "assembled_units",
  //        QString::number(CurrentPallet.value("assembled_units").toInt() -
  //        1));
  //    CurrentPallet.insert("ready_indicator", "false");
  //    CurrentPallet.insert("shipped_indicator", "false");
  //    CurrentPallet.insert("assembling_start", "NULL");
  //    CurrentPallet.insert("assembling_end", "NULL");
  //    if (!Database->updateRecords("pallets", CurrentPallet)) {
  //      sendLog(QString("Получена ошибка при снятии статуса готовности "
  //                      "паллеты '%1'. ")
  //                  .arg(CurrentPallet.value("id")));
  //      return DatabaseQueryError;
  //    }
  //  }

  //  if (CurrentBox.value("assembled_units").toInt() > 0) {
  //    CurrentBox.insert(
  //        "assembled_units",
  //        QString::number(CurrentBox.value("assembled_units").toInt() - 1));
  //  }
  //  CurrentBox.insert("ready_indicator", "false");
  //  CurrentBox.insert("assembling_start", "NULL");
  //  CurrentBox.insert("assembling_end", "NULL");
  //  CurrentBox.insert("production_line_id", "NULL");
  //  if (!Database->updateRecords("boxes", CurrentBox)) {
  //    sendLog(QString("Получена ошибка при уменьшении количества собранных "
  //                    "транспондеров в боксе %1. ")
  //                .arg(CurrentBox.value("id")));
  //    return DatabaseQueryError;
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundBoxManually(
    const QString& id) {
  //  QHash<QString, QString> transponderRecord;

  //  while (1) {
  //    transponderRecord.insert("id", "");
  //    transponderRecord.insert("box_id", id);
  //    transponderRecord.insert("release_counter", ">=1");
  //    if (!Database->getRecordByPart("transponders", transponderRecord)) {
  //      sendLog(QString("Получена ошибка при поиске транспондера в боксе %1.
  //      ")
  //                  .arg(transponderRecord.value("box_id")));
  //      return DatabaseQueryError;
  //    }

  //    if (transponderRecord.isEmpty()) {
  //      sendLog(
  //          QString("Все транспондеры из бокса %1 были возвращены.
  //          ").arg(id));
  //      break;
  //    }

  //    ReturnStatus ret =
  //    refundTransponderManually(transponderRecord.value("id")); if (ret !=
  //    Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundPalletManually(
    const QString& id) {
  //  QHash<QString, QString> boxRecord;

  //  while (1) {
  //    boxRecord.insert("id", "");
  //    boxRecord.insert("pallet_id", id);
  //    boxRecord.insert("assembled_units", ">=1");
  //    if (!Database->getRecordByPart("boxes", boxRecord)) {
  //      sendLog(
  //          QString("Получена ошибка при поиске бокса в паллете %1.
  //          ").arg(id));
  //      return DatabaseQueryError;
  //    }

  //    if (boxRecord.isEmpty()) {
  //      sendLog(QString("Все боксы из паллеты %1 были возвращены. ").arg(id));
  //      break;
  //    }

  //    ReturnStatus ret = refundBoxManually(boxRecord.value("id"));
  //    if (ret != Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundOrderManually(
    const QString& id) {
  //  QHash<QString, QString> palletRecord;

  //  while (1) {
  //    palletRecord.insert("id", "");
  //    palletRecord.insert("order_id", id);
  //    palletRecord.insert("assembled_units", ">=1");
  //    if (!Database->getRecordByPart("pallets", palletRecord)) {
  //      sendLog(
  //          QString("Получена ошибка при поиске паллеты в заказе %1.
  //          ").arg(id));
  //      return DatabaseQueryError;
  //    }

  //    if (palletRecord.isEmpty()) {
  //      sendLog(QString("Все паллеты из заказа %1 были возвращены.
  //      ").arg(id)); break;
  //    }

  //    ReturnStatus ret = refundPalletManually(palletRecord.value("id"));
  //    if (ret != Completed) {
  //      return ret;
  //    }
  //  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::shipPallet(
    const QString& id,
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
    return DatabaseQueryError;
  }

  for (int32_t i = 0; i < records.recordCount(); i++) {
    // Удаляем пробелы из названия модели
    QString tempModel = records.get(i, "transponder_model");

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

  return Completed;
}
