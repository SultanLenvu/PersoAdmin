#include "administration_system.h"
#include "Log/log_system.h"

AdministrationSystem::AdministrationSystem(QObject* parent) : QObject(parent) {
  setObjectName("AdministrationSystem");
  loadSettings();

  createDatabaseController();
}

void AdministrationSystem::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
  Database->applySettings();
}

AdministrationSystem::ReturnStatus AdministrationSystem::connectDatabase() {
  sendLog("Подключение к базе данных. ");
  if (!NewDatabase->connect()) {
    sendLog("Не удалось установить соединение с базой данных. ");
    return DatabaseConnectionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::disconnectDatabase() {
  sendLog("Отключение от базы данных. ");
  Database->disconnect();

  return Completed;
}

void AdministrationSystem::createDatabaseController() {
  Database = new PostgresController(this, "AdministratorConnection");
  connect(Database, &IDatabaseController::logging, LogSystem::instance(),
          &LogSystem::generate);

  NewDatabase = new PostgreSqlDatabase(this, "AdministratorNewConnection");
  connect(NewDatabase, &AbstractSqlDatabase::logging, LogSystem::instance(),
          &LogSystem::generate);
}

AdministrationSystem::ReturnStatus AdministrationSystem::clearDatabaseTable(
    const QString& tableName) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->clearTable(tableName)) {
    sendLog(QString("Получена ошибка при очистке таблицы %1. ").arg(tableName));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Таблица %1 очищена. ").arg(tableName));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getDatabaseTable(
    const QString& tableName,
    DatabaseTableModel* buffer) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->getTable(tableName, 10, buffer)) {
    sendLog(QString("Получена ошибка при получении данных из таблицы %1. ")
                .arg(tableName));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Данные из таблицы %1 успешно получены. ").arg(tableName));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getCustomResponse(
    const QString& req,
    DatabaseTableModel* buffer) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->execCustomRequest(req, buffer)) {
    sendLog("Получена ошибка при выполнении кастомного запроса. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Кастомный запрос успешно выполнен. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::createNewOrder(
    const QSharedPointer<QHash<QString, QString> > orderParameters) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Добавление заказа. ");
  if (!addOrder(orderParameters)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Добавление палет. ");
  if (!addPallets(orderParameters)) {
    sendLog("Получена ошибка при добавлении палет. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Добавление боксов. ");
  if (!addBoxes(orderParameters)) {
    sendLog("Получена ошибка при добавлении боксов. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Добавление транспондеров. ");
  if (!addTransponders(orderParameters)) {
    sendLog("Получена ошибка при добавлении транспондеров. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Новый заказ успешно создан. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::startOrderAssembling(
    const QString& orderId) {
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> productionLineRecord;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Ищем все неактивные производственные линии
  while (1) {
    productionLineRecord.insert("login", "");
    productionLineRecord.insert("password", "");
    productionLineRecord.insert("transponder_id", "");
    productionLineRecord.insert("id", "");
    productionLineRecord.insert("active", "false");
    if (!Database->getRecordByPart("production_lines", productionLineRecord)) {
      sendLog(QString("Получена ошибка при поиске неактивной "
                      "производственной линии '%1'. ")
                  .arg(productionLineRecord.value("login")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если незадействованная линия не найдена
    if (productionLineRecord.isEmpty()) {
      sendLog(QString("Все свободные производственные линии задействованы. "));
      break;
    }

    // Ищем бокс для производственной линии
    if (!searchBoxForProductionLine(orderId, productionLineRecord.value("id"),
                                    boxRecord)) {
      sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                  .arg(boxRecord.value("id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если свободных боксов не найдено
    if (boxRecord.isEmpty()) {
      sendLog(QString("В заказе %1 все боксы заняты. ").arg(orderId));
      break;
    }

    // Запускаем сборку бокса
    if (!startBoxProcessing(boxRecord.value("id"),
                            productionLineRecord.value("id"))) {
      sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                  .arg(boxRecord.value("id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    boxRecord.clear();
    productionLineRecord.clear();
  }

  sendLog(QString("Сборка заказа %1 запущена. ").arg(orderId));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::stopOrderAssembling(
    const QString& orderId) {
  QHash<QString, QString> conditions;
  QHash<QString, QString> newValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    Database->abortTransaction();
    return DatabaseTransactionError;
  }

  sendLog("Деактивация линий производства. ");
  conditions.insert("active", "true");
  newValues.insert("active", "false");
  newValues.insert("transponder_id", "NULL");
  if (!Database->updateAllRecordsByPart("production_lines", conditions,
                                        newValues)) {
    sendLog(QString("Получена ошибка при остановке линий производства. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog("Остановка процесса сборки всех боксов. ");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("boxes", conditions, newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех боксов. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog("Остановка процесса сборки всех паллет.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("pallets", conditions, newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех паллет."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog(QString("Остановка процесса сборки заказа %1.").arg(orderId));
  newValues.insert("id", orderId);
  newValues.insert("in_process", "false");
  if (!Database->updateRecordById("orders", newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки заказа %1.")
                .arg(orderId));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Сборка заказа %1 остановлена. ").arg(orderId));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::deleteLastOrder() {
  QHash<QString, QString> conditions;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Удаление последнего добавленного заказа. ");
  conditions.insert("in_process", "false");
  conditions.insert("ready_indicator", "false");
  Database->removeLastRecordByCondition("orders", conditions);

  sendLog("Последний добавленный заказ успешно удален. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::createNewProductionLine(
    const QHash<QString, QString>* productionLineParameters) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Добавляем линию производства
  sendLog("Добавление линии производства. ");
  if (!addProductionLine(productionLineParameters)) {
    sendLog("Получена ошибка при добавлении производственной линии. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Новая линия производства успешно создана. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::deleteLastProductionLine() {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!removeLastProductionLine()) {
    sendLog("Получена ошибка при удалении последней линии производства. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Последняя линия производства успешно удалена. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::linkProductionLineWithBox(
    const QHash<QString, QString>* linkParameters) {
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> newBoxRecord;
  QHash<QString, QString> transponderRecord;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Запрашиваем данные о производственной линии
  productionLineRecord.insert("login", linkParameters->value("login"));
  productionLineRecord.insert("password", linkParameters->value("password"));
  productionLineRecord.insert("transponder_id", "");
  productionLineRecord.insert("id", "");
  if (!Database->getRecordByPart("production_lines", productionLineRecord)) {
    sendLog(
        QString(
            "Получена ошибка при поиске данных производственной линии '%1'. ")
            .arg(productionLineRecord.value("login")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Получаем данные о новом боксе
  newBoxRecord.insert("id", linkParameters->value("box_id"));
  newBoxRecord.insert("ready_indicator", "");
  newBoxRecord.insert("in_process", "");
  newBoxRecord.insert("pallet_id", "");
  if (!Database->getRecordById("boxes", newBoxRecord)) {
    sendLog(QString("Получена ошибка при поиске данных бокса %1. ")
                .arg(linkParameters->value("box_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Если бокс не найден, то связывание недопустимо
  if (newBoxRecord.isEmpty()) {
    sendLog(QString("Бокс %1 не существует, связывание с "
                    "производственной линией %2 невозможно. ")
                .arg(linkParameters->value("box_id"),
                     productionLineRecord.value("id")));
    return LogicError;
  }

  // Если бокс уже собран, то связывание недопустимо
  if (newBoxRecord.value("ready_indicator") == "true") {
    sendLog(QString("Бокс %1 уже собран, связывание с "
                    "производственной линией %2 невозможно. ")
                .arg(linkParameters->value("box_id"),
                     productionLineRecord.value("id")));
    return LogicError;
  }
  // Если бокс занят другой производственной линией, то связывание
  // недопустимо
  if (newBoxRecord.value("in_process") == "true") {
    sendLog(QString("Бокс %1 уже в процессе сборки, связывание с "
                    "производственной линией %2 невозможно. ")
                .arg(linkParameters->value("box_id"),
                     productionLineRecord.value("id")));
    return LogicError;
  }

  // Получаем данные текущего транспондера, если он был определен
  if (productionLineRecord.value("transponder_id").toInt() > 0) {
    transponderRecord.insert("id",
                             productionLineRecord.value("transponder_id"));
    transponderRecord.insert("box_id", "");
    if (!Database->getRecordById("transponders", transponderRecord)) {
      sendLog(QString("Получена ошибка при поиске данных транспондера %1. ")
                  .arg(productionLineRecord.value("transponder_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Останавливаем сборку соответсвующего бокса
    if (!stopBoxProcessing(transponderRecord.value("box_id"))) {
      sendLog(QString("Получена ошибка при остановке сборки бокса %1. ")
                  .arg(transponderRecord.value("box_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }
  }

  // Запускаем процесс сборки бокса
  if (!startBoxProcessing(newBoxRecord.value("id"),
                          productionLineRecord.value("id"))) {
    sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                .arg(newBoxRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Линия производства %1 успешно связана с боксом %2. ")
              .arg(productionLineRecord.value("id"), newBoxRecord.value("id")));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::shutdownAllProductionLines() {
  QHash<QString, QString> conditions;
  QHash<QString, QString> newValues;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  sendLog("Деактивация линий производства. ");
  conditions.insert("active", "true");
  newValues.insert("active", "false");
  newValues.insert("transponder_id", "NULL");
  if (!Database->updateAllRecordsByPart("production_lines", conditions,
                                        newValues)) {
    sendLog(QString("Получена ошибка при остановке линий производства. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog("Остановка процесса сборки всех боксов. ");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("boxes", conditions, newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех боксов. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog("Остановка процесса сборки всех паллет.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("pallets", conditions, newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех паллет."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog("Остановка процесса сборки всех заказов.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("orders", conditions, newValues)) {
    sendLog(QString("Получена ошибка при остановке сборки всех заказов."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  sendLog(QString("Все линии производства остановлены. "));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::allocateInactiveProductionLines(const QString& orderId) {
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Ищем все неактивные производственные линии
  while (1) {
    productionLineRecord.insert("login", "");
    productionLineRecord.insert("password", "");
    productionLineRecord.insert("transponder_id", "");
    productionLineRecord.insert("id", "");
    productionLineRecord.insert("active", "false");
    if (!Database->getRecordByPart("production_lines", productionLineRecord)) {
      sendLog(QString("Получена ошибка при поиске неактивной "
                      "производственной линии '%1'. ")
                  .arg(productionLineRecord.value("login")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если незадействованная линия не найдена
    if (productionLineRecord.isEmpty()) {
      sendLog(QString("Все производственные линии активны. "));
      if (!Database->closeTransaction()) {
        sendLog("Получена ошибка при закрытии транзакции. ");
        return DatabaseTransactionError;
      }
      return Completed;
    }

    // Ищем свободные боксы в заказе
    tables.append("transponders");
    tables.append("boxes");
    tables.append("pallets");
    foreignKeys.append("box_id");
    foreignKeys.append("pallet_id");
    mergedRecord.insert("transponders.id", "");
    mergedRecord.insert("transponders.box_id", "");
    mergedRecord.insert("boxes.in_process", "false");
    mergedRecord.insert("boxes.ready_indicator", "false");
    mergedRecord.insert("pallets.order_id", orderId);
    if (!Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
      sendLog(
          QString("Получена ошибка при поиске свободного бокса в заказе %1. "));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если свободных боксов в заказе не нашлось
    if (mergedRecord.isEmpty()) {
      break;
    }

    // Связываем линию производства с первым транспондером в найденном свободном
    // боксе
    productionLineRecord.insert("transponder_id", mergedRecord.value("id"));
    productionLineRecord.insert("active", "true");
    if (!Database->updateRecordById("production_lines", productionLineRecord)) {
      sendLog(QString("Получена ошибка при активации производственной линии "
                      "производственной линии %1. ")
                  .arg(productionLineRecord.value("if")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Запускаем сборку бокса
    if (!startBoxProcessing(mergedRecord.value("box_id"),
                            productionLineRecord.value("id"))) {
      sendLog(QString("Получена ошибка при запуске сборки бокса %1. ")
                  .arg(mergedRecord.value("box_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    productionLineRecord.clear();
    mergedRecord.clear();
    foreignKeys.clear();
    tables.clear();
  }

  sendLog(QString("Все производственные линии активны. "));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::initIssuerTable() {
  QHash<QString, QString> record;
  int32_t lastId = 0;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Получаем идентифкатор последнего добавленного заказа
  record.insert("id", "");
  if (!Database->getLastRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  lastId = record.value("id").toInt();

  sendLog("Инициализация таблицы эмитентов. ");
  record.insert("id", QString::number(++lastId));
  record.insert("name", "Пауэр Синтез");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Автодор");
  record.insert("efc_context_mark", "570002FF0070");
  if (!Database->addRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Новое качество дорог");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Магистраль северной столицы");
  record.insert("efc_context_mark", "570001FF0070");
  if (!Database->addRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Объединенные системы сбора платы");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    sendLog("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  sendLog("Таблица эмитентов успешно инициализирована. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::initTransportMasterKeysTable() {
  QHash<QString, QString> transportKeysRecord;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  transportKeysRecord.insert("id", "");
  if (!Database->getLastRecord("transport_master_keys", transportKeysRecord)) {
    sendLog("Получена ошибка при поиске последнего заказа. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Конструируем запись
  transportKeysRecord.insert(
      "id", QString::number(transportKeysRecord.value("id").toInt() + 1));
  transportKeysRecord.insert("accr_key", TRANSPORT_MACCRKEY_DEFAULT_VALUE);
  transportKeysRecord.insert("per_key", TRANSPORT_MPERKEY_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key1", TRANSPORT_MAUKEY1_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key2", TRANSPORT_MAUKEY2_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key3", TRANSPORT_MAUKEY3_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key4", TRANSPORT_MAUKEY4_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key5", TRANSPORT_MAUKEY5_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key6", TRANSPORT_MAUKEY6_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key7", TRANSPORT_MAUKEY7_DEFAULT_VALUE);
  transportKeysRecord.insert("au_key8", TRANSPORT_MAUKEY8_DEFAULT_VALUE);

  // Если транспортные ключи для эмитента отсутствуют, то создаем новую запись
  if (!Database->addRecord("transport_master_keys", transportKeysRecord)) {
    sendLog(
        QString("Получена ошибка при добавлении транспортных мастер ключей. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog("Таблица транспортных ключей успешно инициализирована. ");
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::linkIssuerWithMasterKeys(
    const QHash<QString, QString>* linkParameters) {
  QHash<QString, QString> issuerRecord;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    Database->abortTransaction();
    return DatabaseTransactionError;
  }

  sendLog(QString("Обновление записи эмитента %1.")
              .arg(linkParameters->value("issuer_id")));
  issuerRecord.insert("id", linkParameters->value("issuer_id"));
  if (linkParameters->value("master_keys_type") == "transport_master_keys") {
    issuerRecord.insert("transport_master_keys_id",
                        linkParameters->value("master_keys_id"));
  } else {
    issuerRecord.insert("commercial_master_keys_id",
                        linkParameters->value("master_keys_id"));
  }

  if (!Database->updateRecordById("issuers", issuerRecord)) {
    sendLog(QString("Получена ошибка при обновлении записи эмитента %1.")
                .arg(linkParameters->value("issuer_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  sendLog(QString("Эмитент %1 успешно связан с мастер ключами %2. ")
              .arg(linkParameters->value("issuer_id"),
                   linkParameters->value("master_keys_id")));
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->abortTransaction();
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
  data->insert("box_id", CurrentBox.value("id"));
  data->insert("pallet_id", CurrentPallet.value("id"));
  data->insert("order_id", CurrentOrder.value("id"));

  // Удаляем пробелы из названия модели
  QString tempModel = CurrentOrder.value("transponder_model");
  data->insert("transponder_model", tempModel.remove(" "));

  // Преобразуем в десятичный формат
  QString manufacturerId =
      QString::number(CurrentOrder.value("manufacturer_id").toInt(nullptr, 16));

  // Дата сборки
  QStringList tempDate = CurrentBox.value("assembling_start").split("T");
  QDate date = QDate::fromString(tempDate.first(), "yyyy-MM-dd");
  QString batteryInsertationDate =
      QString("%1%2")
          .arg(QString::number(date.weekNumber()), 2, QChar('0'))
          .arg(QString::number(date.year() % 100), 2, QChar('0'));

  // Дополняем серийник до 10 цифр нулями слева
  QString extendedTransponderId =
      QString("%1").arg(CurrentTransponder.value("id"), 10, QChar('0'));

  // Конструируем серийный номер транспондера
  data->insert("sn",
               QString("%1%2%3").arg(manufacturerId, batteryInsertationDate,
                                     extendedTransponderId));

  // Вычленяем символ F из personal_account_number
  QString tempPan = CurrentTransponder.value("personal_account_number");
  data->insert("pan", tempPan.remove(QChar('F')));

  // Название компании-заказчика
  data->insert("issuer_name", CurrentIssuer.value("name"));

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getBoxData(
    const QString& id,
    QHash<QString, QString>* data) {
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> transponderRecord;
  QHash<QString, QString> transponderData;

  boxRecord.insert("id", id);
  boxRecord.insert("assembled_units", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    emit logging(QString("Получена ошибка при поиске бокса с id %1. ").arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем данные бокса
  data->insert("id", id);
  data->insert("assembled_units", boxRecord.value("assembled_units"));

  // Ищем первый транспондер в боксе
  transponderRecord.insert("id", "");
  transponderRecord.insert("box_id", id);
  if (!Database->getRecordByPart("transponders", transponderRecord, true)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponderRecord.value("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponderRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем серийник первого транспондера в боксе
  data->insert("first_transponder_sn", transponderData.value("sn"));
  transponderData.clear();
  transponderRecord.clear();

  // Ищем последний транспондер в боксе
  transponderRecord.insert("id", "");
  transponderRecord.insert("box_id", id);
  if (!Database->getRecordByPart("transponders", transponderRecord, false)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponderRecord.value("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponderRecord.value("id")));
    Database->abortTransaction();
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
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> palletRecord;
  QHash<QString, QString> orderRecord;

  palletRecord.insert("id", id);
  palletRecord.insert("assembled_units", "");
  palletRecord.insert("order_id", "");
  palletRecord.insert("assembling_end", "");
  if (!Database->getRecordById("pallets", palletRecord)) {
    emit logging(
        QString("Получена ошибка при поиске паллеты с id %1. ").arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  orderRecord.insert("id", palletRecord.value("order_id"));
  orderRecord.insert("transponder_model", "");
  if (!Database->getRecordById("orders", orderRecord)) {
    emit logging(QString("Получена ошибка при поиске заказа с id %1. ")
                     .arg(palletRecord.value("order_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем данные паллеты
  data->insert("id", id);
  QStringList tempDate = palletRecord.value("assembling_end").split("T");
  data->insert(
      "assembly_date",
      QDate::fromString(tempDate.first(), "yyyy-MM-dd").toString("dd.MM.yyyy"));
  QString tempModel = orderRecord.value("transponder_model");
  data->insert("transponder_model", tempModel.remove(" "));

  // Ищем первый бокс в паллете
  boxRecord.insert("id", "");
  boxRecord.insert("pallet_id", id);
  if (!Database->getRecordByPart("boxes", boxRecord, true)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем идентификатор первого бокса
  data->insert("first_box_id", boxRecord.value("id"));
  boxRecord.clear();

  // Ищем последний бокс в паллете
  boxRecord.insert("id", "");
  boxRecord.insert("quantity", "");
  boxRecord.insert("pallet_id", id);
  if (!Database->getRecordByPart("boxes", boxRecord, false)) {
    emit logging(
        QString("Получена ошибка при поиске первого транспондера в боксе %1. ")
            .arg(id));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Сохраняем идентификатор последнего бокса
  data->insert("last_box_id", boxRecord.value("id"));
  uint32_t totalQuantity = palletRecord.value("assembled_units").toInt() *
                           boxRecord.value("quantity").toInt();
  data->insert("quantity", QString::number(totalQuantity));

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::rollbackProductionLine(
    const QString& id) {
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> transponderRecord;
  QHash<QString, QString> boxRecord;

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  productionLineRecord.insert("login", "");
  productionLineRecord.insert("password", "");
  productionLineRecord.insert("transponder_id", "");
  productionLineRecord.insert("id", id);
  productionLineRecord.insert("transponder_id", "");
  if (!Database->getRecordById("production_lines", productionLineRecord)) {
    sendLog(QString("Получена ошибка при поиске производственной линии '%1'. ")
                .arg(productionLineRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  if (productionLineRecord.isEmpty()) {
    sendLog(
        QString("Производственная линия с идентификатором '%1' не найдена. ")
            .arg(productionLineRecord.value("id")));
    Database->abortTransaction();
    return LogicError;
  }

  transponderRecord.insert("id", productionLineRecord.value("transponder_id"));
  transponderRecord.insert("box_id", "");
  if (!Database->getRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при поиске транспондера '%1'. ")
                .arg(transponderRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  boxRecord.insert("id", transponderRecord.value("box_id"));
  boxRecord.insert("assembled_units", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    sendLog(QString("Получена ошибка при поиске данных бокса '%1'. ")
                .arg(transponderRecord.value("box_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  transponderRecord.insert("id", "");
  transponderRecord.insert("release_counter", ">1");
  if (!Database->getRecordByPart("transponders", transponderRecord, false)) {
    sendLog(QString("Получена ошибка при поиске предыдущего транспондера "
                    "производственной линии в боксе %1. ")
                .arg(transponderRecord.value("box_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  if (transponderRecord.isEmpty()) {
    sendLog(QString("Производственная линия '%1' связана с первым "
                    "транспондером в боксе. Откат невозможен.")
                .arg(productionLineRecord.value("id")));
    Database->abortTransaction();
    return LogicError;
  }

  transponderRecord.insert("release_counter", "0");
  transponderRecord.insert("ucid", "NULL");
  transponderRecord.insert("awaiting_confirmation", "false");
  if (!Database->updateRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при возврате транспондера %1.")
                .arg(transponderRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  transponderRecord.clear();

  boxRecord.insert(
      "assembled_units",
      QString::number(boxRecord.value("assembled_units").toInt() - 1));
  if (!Database->updateRecordById("boxes", boxRecord)) {
    sendLog(QString("Получена ошибка при уменьшении количества собранных "
                    "транспондеров в боксе '%1'. ")
                .arg(transponderRecord.value("box_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  productionLineRecord.insert("transponder_id", transponderRecord.value("id"));
  if (!Database->updateRecordById("production_lines", productionLineRecord)) {
    sendLog(QString("Получена ошибка при связывании производственной линии %1 "
                    "с транспондером %2. ")
                .arg(productionLineRecord.value("id"),
                     transponderRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->abortTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::releaseTranspondersManually(const QString& table,
                                                  const QString& id) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
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
    Database->abortTransaction();
    return ParameterError;
  }

  if (ret != Completed) {
    Database->abortTransaction();
    return ret;
  }

  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->abortTransaction();
    return DatabaseTransactionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::refundTranspondersManually(const QString& table,
                                                 const QString& id) {
  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
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
    Database->abortTransaction();
    return ParameterError;
  }

  if (ret != Completed) {
    Database->abortTransaction();
    return ret;
  }

  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->abortTransaction();
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

  if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии файла реестра отгрузки. ");
    return ShipmentRegisterError;
  }
  file.resize(0);
  out << "model;sn;pan;box_id;pallet_id\n";
  out.flush();

  if (!Database->openTransaction()) {
    sendLog("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  for (uint32_t i = param->value("first_pallet_id").toUInt();
       i <= param->value("last_pallet_id").toUInt(); i++) {
    sendLog(QString("Отгрузка паллеты %1.").arg(QString::number(i)));
    ret = shipPallet(QString::number(i), &file);
    if (ret != Completed) {
      sendLog(QString("Получена ошибка при отгрузке паллеты %1.")
                  .arg(QString::number(i)));
      file.close();
      Database->abortTransaction();
      return DatabaseQueryError;
    }
  }

  file.close();
  if (!Database->closeTransaction()) {
    sendLog("Получена ошибка при закрытии транзакции. ");
    Database->abortTransaction();
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
    emit logging(QString("%1 - %2").arg(objectName(), log));
  }
}

bool AdministrationSystem::getCurrentContext(const QString& id) {
  CurrentTransponder.insert("id", id);
  CurrentTransponder.insert("release_counter", "");
  CurrentTransponder.insert("personal_account_number", "");
  CurrentTransponder.insert("ucid", "");
  CurrentTransponder.insert("awaiting_confirmation", "");
  CurrentTransponder.insert("box_id", "");
  if (!Database->getRecordById("transponders", CurrentTransponder)) {
    sendLog(QString("Получена ошибка при получении данных транспондера %1. ")
                .arg(id));
    return false;
  }

  CurrentBox.insert("id", CurrentTransponder.value("box_id"));
  CurrentBox.insert("quantity", "");
  CurrentBox.insert("ready_indicator", "");
  CurrentBox.insert("in_process", "");
  CurrentBox.insert("assembled_units", "");
  CurrentBox.insert("assembling_start", "");
  CurrentBox.insert("assembling_end", "");
  CurrentBox.insert("pallet_id", "");
  CurrentBox.insert("production_line_id", "");
  if (!Database->getRecordById("boxes", CurrentBox)) {
    sendLog(QString("Получена ошибка при получении данных бокса %1. ")
                .arg(CurrentTransponder.value("box_id")));
    return false;
  }

  CurrentPallet.insert("id", CurrentBox.value("pallet_id"));
  CurrentPallet.insert("quantity", "");
  CurrentPallet.insert("ready_indicator", "");
  CurrentPallet.insert("in_process", "");
  CurrentPallet.insert("assembled_units", "");
  CurrentPallet.insert("assembling_start", "");
  CurrentPallet.insert("assembling_end", "");
  CurrentPallet.insert("shipped_indicator", "");
  CurrentPallet.insert("order_id", "");
  if (!Database->getRecordById("pallets", CurrentPallet)) {
    sendLog(QString("Получена ошибка при получении данных паллеты %1. ")
                .arg(CurrentBox.value("pallet_id")));
    return false;
  }

  CurrentOrder.insert("id", CurrentPallet.value("order_id"));
  CurrentOrder.insert("quantity", "");
  CurrentOrder.insert("full_personalization", "");
  CurrentOrder.insert("ready_indicator", "");
  CurrentOrder.insert("in_process", "");
  CurrentOrder.insert("assembled_units", "");
  CurrentOrder.insert("assembling_start", "");
  CurrentOrder.insert("assembling_end", "");
  CurrentOrder.insert("transponder_model", "");
  CurrentOrder.insert("accr_reference", "");
  CurrentOrder.insert("manufacturer_id", "");
  CurrentOrder.insert("equipment_class", "");
  CurrentOrder.insert("shipped_units", "");
  CurrentOrder.insert("fully_shipped", "");
  CurrentOrder.insert("issuer_id", "");
  if (!Database->getRecordById("orders", CurrentOrder)) {
    sendLog(QString("Получена ошибка при получении данных заказа %1. ")
                .arg(CurrentPallet.value("order_id")));
    return false;
  }

  CurrentIssuer.insert("id", CurrentOrder.value("issuer_id"));
  CurrentIssuer.insert("name", "");
  CurrentIssuer.insert("efc_context_mark", "");
  CurrentIssuer.insert("transport_master_keys_id", "");
  CurrentIssuer.insert("commercial_master_keys_id", "");
  if (!Database->getRecordById("issuers", CurrentIssuer)) {
    sendLog(QString("Получена ошибка при получении данных заказчика %1. ")
                .arg(CurrentOrder.value("issuer_id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::addOrder(
    const QSharedPointer<QHash<QString, QString> > orderParameters) const {
  QHash<QString, QString> issuerRecord;
  QHash<QString, QString> orderRecord;
  int32_t lastId = 0;

  issuerRecord.insert("id", "");
  issuerRecord.insert("name", orderParameters->value("issuer_name"));
  if (!Database->getRecordByPart("issuers", issuerRecord)) {
    sendLog(QString("Не найден идентифкатор эмитента \"%1\".")
                .arg(orderParameters->value("IssuerName")));
    return false;
  }

  orderRecord.insert("id", "");
  if (!Database->getLastRecord("orders", orderRecord)) {
    sendLog("Получена ошибка при поиске последнего заказа. ");
    return false;
  }
  lastId = orderRecord.value("id").toInt();

  // Формируем новую запись
  orderRecord.insert("id", QString::number(lastId + 1));
  orderRecord.insert("issuer_id", issuerRecord.value("id"));
  orderRecord.insert("quantity", "0");
  orderRecord.insert("full_personalization",
                     orderParameters->value("full_personalization"));
  orderRecord.insert(
      "transponder_model",
      orderParameters->value("transponder_model")
          .rightJustified(TRANSPONDER_MODEL_CHAR_LENGTH, QChar(' ')));
  orderRecord.insert("accr_reference",
                     orderParameters->value("accr_reference"));
  orderRecord.insert("equipment_class",
                     orderParameters->value("equipment_class"));
  orderRecord.insert("manufacturer_id",
                     orderParameters->value("manufacturer_id"));
  if (!Database->addRecord("orders", orderRecord)) {
    sendLog("Получена ошибка при добавлении заказа. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::addPallets(
    const QSharedPointer<QHash<QString, QString> > orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  QHash<QString, QString> orderRecord;
  QHash<QString, QString> palletRecord;
  int32_t lastId = 0;

  // Поиск идентификатора незаполненного заказа
  orderRecord.insert("id", "");
  orderRecord.insert("quantity", "0");
  if (!Database->getRecordByPart("orders", orderRecord)) {
    sendLog(
        "Получена ошибка при поиске идентификатора незаполненного заказа. ");
    return false;
  }

  // Заполняем заказ
  for (uint32_t i = 0; i < orderCapacity; i++) {
    // Получаем идентификатор последней палеты
    palletRecord.insert("id", "");
    if (!Database->getLastRecord("pallets", palletRecord)) {
      sendLog("Получена ошибка при поиске последней палеты. ");
      return false;
    }
    lastId = palletRecord.value("id").toInt();

    // Формируем новую запись
    palletRecord.insert("id", QString::number(lastId + 1));
    palletRecord.insert("quantity", "0");
    palletRecord.insert("order_id", orderRecord.value("id"));
    // Добавляем новую запись
    if (!Database->addRecord("pallets", palletRecord)) {
      sendLog("Получена ошибка при добавлении палеты. ");
      return false;
    }
  }

  // Заполнение заказа
  orderRecord.insert("quantity", QString::number(orderCapacity));
  if (!Database->updateRecordById("orders", orderRecord)) {
    sendLog(QString("Получена ошибка при заполнении заказа %1. ")
                .arg(orderRecord.value("id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::addBoxes(
    const QSharedPointer<QHash<QString, QString> > orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t palletCount = transponderCount / (palletCapacity * boxCapacity);
  QHash<QString, QString> palletRecord;
  QHash<QString, QString> boxRecord;
  int32_t lastId = 0;

  for (uint32_t i = 0; i < palletCount; i++) {
    // Получаем идентификатор незаполненной палеты
    palletRecord.insert("id", "");
    palletRecord.insert("quantity", "0");
    if (!Database->getRecordByPart("pallets", palletRecord)) {
      sendLog(
          "Получена ошибка при поиске идентификатора незаполненной палеты. ");
      return false;
    }

    // Создаем боксы
    for (uint32_t i = 0; i < palletCapacity; i++) {
      // Получаем идентификатор последнего добавленного бокса
      boxRecord.insert("id", "");
      if (!Database->getLastRecord("boxes", boxRecord)) {
        sendLog("Получена ошибка при поиске последнего бокса. ");
        return false;
      }
      lastId = boxRecord.value("id").toInt();

      // Формируем новую запись
      boxRecord.insert("id", QString::number(lastId + 1));
      boxRecord.insert("quantity", "0");
      boxRecord.insert("pallet_id", palletRecord.value("id"));
      // Добавляем новую запись
      if (!Database->addRecord("boxes", boxRecord)) {
        sendLog("Получена ошибка при добавлении бокса. ");
        return false;
      }
    }

    // Заполнение палеты
    palletRecord.insert("quantity", QString::number(palletCapacity));
    if (!Database->updateRecordById("pallets", palletRecord)) {
      sendLog(QString("Получена ошибка при заполнении палеты %1. ")
                  .arg(palletRecord.value("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::addTransponders(
    const QSharedPointer<QHash<QString, QString> > orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t boxCount = transponderCount / boxCapacity;
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> transponderRecord;
  int32_t lastId = 0;

  QFile panFile(orderParameters->value("pan_file_path"));
  if (!panFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    sendLog("Получена ошибка при открытии PAN-файла. ");
    return false;
  }
  QTextStream in(&panFile);

  for (uint32_t i = 0; i < boxCount; i++) {
    // Получаем идентификатор незаполненного бокса
    boxRecord.insert("id", "");
    boxRecord.insert("quantity", "0");
    if (!Database->getRecordByPart("boxes", boxRecord)) {
      sendLog(
          "Получена ошибка при поиске идентификатора незаполненного бокса. ");
      return false;
    }

    // Создаем транспондеры
    for (uint32_t i = 0; i < boxCapacity; i++) {
      // Получаем идентификатор последнего добавленного транспондера
      transponderRecord.insert("id", "");
      if (!Database->getLastRecord("transponders", transponderRecord)) {
        sendLog("Получена ошибка при поиске последнего бокса. ");
        return false;
      }
      lastId = transponderRecord.value("id").toInt();

      // Формируем новую запись
      transponderRecord.insert("id", QString::number(lastId + 1));
      transponderRecord.insert(
          "personal_account_number",
          in.readLine().leftJustified(FULL_PAN_CHAR_LENGTH, QChar('F')));
      transponderRecord.insert("release_counter", "0");
      transponderRecord.insert("box_id", boxRecord.value("id"));

      // Добавляем новую запись
      if (!Database->addRecord("transponders", transponderRecord)) {
        sendLog("Получена ошибка при добавлении транспондера. ");
        return false;
      }
    }

    // Заполнение бокса
    boxRecord.insert("quantity", QString::number(boxCapacity));
    if (!Database->updateRecordById("boxes", boxRecord)) {
      sendLog(QString("Получена ошибка при заполнении бокса %1. ")
                  .arg(boxRecord.value("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::addProductionLine(
    const QHash<QString, QString>* productionLineParameters) const {
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;
  int32_t lastId = 0;

  // Ищем первый транспондер в свободном боксе
  tables.append("transponders");
  tables.append("boxes");
  foreignKeys.append("box_id");
  mergedRecord.insert("transponders.id", "");
  mergedRecord.insert("transponders.box_id", "");
  mergedRecord.insert("boxes.in_process", "false");
  mergedRecord.insert("boxes.ready_indicator", "false");
  if (!Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
    sendLog(
        "Получена ошибка при поиске первого транспондера свободного бокса. ");
    return false;
  }

  // Получаем идентификатор последней линии производства
  productionLineRecord.insert("id", "");
  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
    sendLog(
        "Получена ошибка при поиске последней созданной линии производства. ");
    return false;
  }
  lastId = productionLineRecord.value("id").toInt();

  // Добавляем новую линию производства
  productionLineRecord = *productionLineParameters;
  productionLineRecord.insert("id", QString::number(lastId + 1));
  productionLineRecord.insert("transponder_id", "NULL");
  productionLineRecord.insert("active", "false");
  if (!Database->addRecord("production_lines", productionLineRecord)) {
    sendLog("Получена ошибка при добавлении линии производства. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::startBoxProcessing(
    const QString& id,
    const QString& productionLineId) const {
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> transponderRecord;

  // Получаем данные о боксе
  boxRecord.insert("id", id);
  boxRecord.insert("pallet_id", "");
  boxRecord.insert("in_process", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    sendLog("Получена ошибка при поиске данных о боксе. ");
    return false;
  }

  // Ищем в боксе первый невыпущенный транспондер в боксе
  transponderRecord.insert("id", "");
  transponderRecord.insert("release_counter", "0");
  transponderRecord.insert("box_id", id);
  if (!Database->getRecordByPart("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при поиске "
                    "невыпущенного транспондера в боксе %1. ")
                .arg(id));
    return false;
  }

  // Связываем транспондер с производственной линией
  productionLineRecord.insert("id", productionLineId);
  productionLineRecord.insert("transponder_id", transponderRecord.value("id"));
  productionLineRecord.insert("active", "true");
  if (!Database->updateRecordById("production_lines", productionLineRecord)) {
    sendLog(
        QString("Получена ошибка при связывании производственной линии %1 с "
                "транспондером %2. ")
            .arg(productionLineRecord.value("id"),
                 transponderRecord.value("id")));
    return false;
  }

  // Связываем бокс с производственной линией и обновляем время начала сборки
  if (boxRecord.value("in_process") != "true") {
    boxRecord.insert("in_process", "true");
    boxRecord.insert("assembling_start", QDateTime::currentDateTime().toString(
                                             POSTGRES_TIMESTAMP_TEMPLATE));
    boxRecord.insert("production_line_id", productionLineId);
    if (!Database->updateRecordById("boxes", boxRecord)) {
      sendLog(
          QString("Получена ошибка при связывании производственной линии %1 с "
                  "боксом %2. ")
              .arg(productionLineId, id));
      return false;
    }

    // Запуск сборки палеты
    if (!startPalletProcessing(boxRecord.value("pallet_id"))) {
      sendLog(QString("Не удалось запустить сборку палеты %1. ")
                  .arg(boxRecord.value("pallet_id")));
      return false;
    }
  } else {
    sendLog(
        QString("Бокс %1 уже в процессе сборки.").arg(boxRecord.value("id")));
  }
  return true;
}

bool AdministrationSystem::startPalletProcessing(const QString& id) const {
  QHash<QString, QString> palletRecord;

  palletRecord.insert("id", id);
  palletRecord.insert("order_id", "");
  palletRecord.insert("in_process", "");
  if (!Database->getRecordById("pallets", palletRecord)) {
    sendLog("Получена ошибка при поиске данных о паллете. ");
    return false;
  }

  if (palletRecord.value("in_process") != "true") {
    palletRecord.insert("in_process", "true");
    palletRecord.insert(
        "assembling_start",
        QDateTime::currentDateTime().toString(POSTGRES_TIMESTAMP_TEMPLATE));
    if (!Database->updateRecordById("pallets", palletRecord)) {
      sendLog("Получена ошибка при запуске сборки палеты. ");
      return false;
    }

    // Запуск сборки заказа
    if (!startOrderProcessing(palletRecord.value("order_id"))) {
      sendLog(QString("Не удалось запустить сборку заказа. ")
                  .arg(palletRecord.value("order_id")));
      return false;
    }
  } else {
    sendLog(QString("Палета %1 уже в процессе сборки. ")
                .arg(palletRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::startOrderProcessing(const QString& id) const {
  QHash<QString, QString> orderRecord;

  orderRecord.insert("id", id);
  if (!Database->getRecordById("orders", orderRecord)) {
    sendLog("Получена ошибка при поиске данных о заказе. ");
    return false;
  }

  if (orderRecord.value("in_process") != "true") {
    orderRecord.insert("in_process", "true");
    orderRecord.insert(
        "assembling_start",
        QDateTime::currentDateTime().toString(POSTGRES_TIMESTAMP_TEMPLATE));
    if (!Database->updateRecordById("orders", orderRecord)) {
      sendLog("Получена ошибка при запуске сборки заказа. ");
      return false;
    }
  } else {
    sendLog(QString("Заказ %1 уже в процессе сборки. ")
                .arg(orderRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::removeLastProductionLine() const {
  QHash<QString, QString> productionLineRecord;
  QHash<QString, QString> boxRecord;

  // Получение последней добавленной линии производства
  productionLineRecord.insert("id", "");
  productionLineRecord.insert("active", "");
  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
    sendLog(
        "Получена ошибка при поиске последней добавленной производственной "
        "линии. ");
    return false;
  }

  // Если производственная линия активна, то удаление невозможно
  if (productionLineRecord.value("active") == "true") {
    sendLog("Производственная линия активна, удаление невозможно. ");
    return false;
  }

  // Удаляем запись из таблицы линий производства
  if (!Database->removeLastRecordById("production_lines")) {
    sendLog("Получена ошибка при удалении последней линии производства. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::stopBoxProcessing(const QString& id) const {
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> mergedRecord;
  QHash<QString, QString> productionLineRecord;
  QStringList tables;
  QStringList foreignKeys;

  // Получаем данные о боксе
  boxRecord.insert("id", id);
  boxRecord.insert("pallet_id", "");
  boxRecord.insert("in_process", "");
  boxRecord.insert("production_line_id", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    sendLog("Получена ошибка при поиске данных о боксе. ");
    return false;
  }

  // Останавливаем производственную линию, связанную с боксом
  productionLineRecord.insert("id", boxRecord.value("production_line_id"));
  productionLineRecord.insert("active", "false");
  productionLineRecord.insert("transponder_id", "NULL");
  if (!Database->updateRecordById("production_lines", productionLineRecord)) {
    sendLog(QString("Получена ошибка при остановке производственной линии %1.")
                .arg(productionLineRecord.value("id")));
    return false;
  }

  if (boxRecord.value("in_process") != "false") {
    boxRecord.insert("in_process", "false");
    if (!Database->updateRecordById("boxes", boxRecord)) {
      sendLog("Получена ошибка при остановке сборки бокса. ");
      return false;
    }

    // Проверка наличия в палете других боксов , находящихся в процессе сборки
    tables.append("boxes");
    tables.append("pallets");
    mergedRecord.insert("boxes.id", "");
    mergedRecord.insert("boxes.in_process", "true");
    mergedRecord.insert("boxes.pallet_id", boxRecord.value("pallet_id"));
    foreignKeys.append("pallet_id");
    if (Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
      // Проверка наличия записей
      if (mergedRecord.isEmpty()) {
        // Остановка сборки паллеты
        sendLog(QString("Остановка сборки паллеты %1. ")
                    .arg(boxRecord.value("pallet_id")));
        if (!stopPalletProcessing(boxRecord.value("pallet_id"))) {
          sendLog("Не удалось остановить сборку палеты. ");
          return false;
        }
      } else {
        sendLog("В паллете еще есть боксы, находящиеся в процессе сборки. ");
      }
    }
  } else {
    sendLog(QString("Бокс %1 не находится в процессе сборки. ")
                .arg(boxRecord.value("id")));
  }
  return true;
}

bool AdministrationSystem::stopPalletProcessing(const QString& id) const {
  QHash<QString, QString> palletRecord;
  QHash<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;

  palletRecord.insert("id", id);
  palletRecord.insert("order_id", "");
  palletRecord.insert("in_process", "");
  if (!Database->getRecordById("pallets", palletRecord)) {
    sendLog("Получена ошибка при поиске данных о паллете. ");
    return false;
  }

  if (palletRecord.value("in_process") != "false") {
    palletRecord.insert("in_process", "false");
    if (!Database->updateRecordById("pallets", palletRecord)) {
      sendLog("Получена ошибка при остановке сборки палеты. ");
      return false;
    }

    // Проверка наличия в заказе других паллет, находящихся в процессе сборки
    tables.append("pallets");
    tables.append("orders");
    mergedRecord.insert("pallets.id", "");
    mergedRecord.insert("pallets.in_process", "true");
    mergedRecord.insert("pallets.order_id", palletRecord.value("order_id"));
    foreignKeys.append("order_id");
    if (Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
      // Проверка наличия записей
      if (mergedRecord.isEmpty()) {
        // Остановка сборки заказа
        sendLog(QString("Остановка сборки заказа %1. ")
                    .arg(palletRecord.value("order_id")));
        if (!stopOrderProcessing(palletRecord.value("order_id"))) {
          sendLog("Не удалось остановить сборку заказа. ");
          return false;
        }
      } else {
        sendLog("В заказе еще есть паллеты, находящиеся в процессе сборки. ");
      }
    }
  } else {
    sendLog(QString("Палета %1 не находится в процессе сборки. ")
                .arg(palletRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::stopOrderProcessing(const QString& id) const {
  QHash<QString, QString> orderRecord;

  orderRecord.insert("id", id);
  if (!Database->getRecordById("orders", orderRecord)) {
    sendLog("Получена ошибка при поиске данных о заказе. ");
    return false;
  }

  if (orderRecord.value("in_process") != "false") {
    orderRecord.insert("in_process", "false");
    if (!Database->updateRecordById("orders", orderRecord)) {
      sendLog("Получена ошибка при остановке сборки заказа. ");
      return false;
    }
  } else {
    sendLog(QString("Заказ %1 не находится в процессе сборки. ")
                .arg(orderRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::searchBoxForProductionLine(
    const QString& orderId,
    const QString& productionLineId,
    QHash<QString, QString>& boxRecord) const {
  QStringList tables;
  QStringList foreignKeys;

  // Ищем незавершенные боксы
  tables.append("boxes");
  tables.append("pallets");
  tables.append("orders");
  foreignKeys.append("pallet_id");
  foreignKeys.append("order_id");

  boxRecord.insert("boxes.id", "");
  boxRecord.insert("boxes.ready_indicator", "false");
  boxRecord.insert("boxes.production_line_id", productionLineId);
  boxRecord.insert("boxes.in_process", "false");
  boxRecord.insert("pallets.order_id", orderId);
  if (!Database->getMergedRecordByPart(tables, foreignKeys, boxRecord)) {
    sendLog(
        QString("Получена ошибка при поиске незавершенного бокса в заказе %1. ")
            .arg(orderId));
    return false;
  }

  // Если незавершенных боксов нет, то ищем свободные боксы
  if (boxRecord.isEmpty()) {
    boxRecord.insert("boxes.id", "");
    boxRecord.insert("boxes.ready_indicator", "false");
    boxRecord.insert("boxes.production_line_id", "");
    boxRecord.insert("boxes.in_process", "false");
    boxRecord.insert("pallets.order_id", orderId);
    if (!Database->getMergedRecordByPart(tables, foreignKeys, boxRecord)) {
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
  if (!getCurrentContext(id)) {
    sendLog(QString("Получена ошибка при получении контекста транспондера %1. ")
                .arg(id));
    return DatabaseQueryError;
  }

  if (CurrentTransponder.value("release_counter").toInt() > 0) {
    sendLog(
        QString("Транспондер уже был выпущен ранее, принудительный выпуск "
                "невозможен. "));
    return LogicError;
  }

  CurrentTransponder.insert(
      "release_counter",
      QString::number(CurrentTransponder.value("release_counter").toInt() + 1));
  CurrentTransponder.insert("ucid", "NULL");
  CurrentTransponder.insert("awaiting_confirmation", "false");
  if (!Database->updateRecordById("transponders", CurrentTransponder)) {
    sendLog(
        QString("Получена ошибка при принудительном выпуске транспондера %1. ")
            .arg(id));
    return DatabaseQueryError;
  }

  CurrentBox.insert(
      "assembled_units",
      QString::number(CurrentBox.value("assembled_units").toInt() + 1));
  CurrentBox.insert("assembling_start", "NULL");
  CurrentBox.insert("assembling_end", "NULL");
  CurrentBox.insert("production_line_id", "NULL");
  if (!Database->updateRecordById("boxes", CurrentBox)) {
    sendLog(QString("Получена ошибка при увеличении количества собранных "
                    "транспондеров в боксе %1. ")
                .arg(CurrentBox.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  if (CurrentBox.value("assembled_units") == CurrentBox.value("quantity")) {
    CurrentBox.insert("ready_indicator", "true");
    if (!Database->updateRecordById("boxes", CurrentBox)) {
      sendLog(QString("Получена ошибка при завершении сборки бокса %1. ")
                  .arg(CurrentBox.value("id")));
      return DatabaseQueryError;
    }

    CurrentPallet.insert(
        "assembled_units",
        QString::number(CurrentPallet.value("assembled_units").toInt() + 1));
    CurrentPallet.insert("assembling_start", "NULL");
    CurrentPallet.insert("assembling_end", "NULL");
    if (!Database->updateRecordById("pallets", CurrentPallet)) {
      sendLog(QString("Получена ошибка при увеличении количества собранных "
                      "боксов в паллете %1. ")
                  .arg(CurrentPallet.value("id")));
      return DatabaseQueryError;
    }

    if (CurrentPallet.value("assembled_units") ==
        CurrentPallet.value("quantity")) {
      CurrentPallet.insert("ready_indicator", "true");
      if (!Database->updateRecordById("pallets", CurrentPallet)) {
        sendLog(QString("Получена ошибка при завершении сборки паллеты %1. ")
                    .arg(CurrentPallet.value("id")));
        return DatabaseQueryError;
      }

      CurrentOrder.insert(
          "assembled_units",
          QString::number(CurrentOrder.value("assembled_units").toInt() + 1));
      CurrentOrder.insert("assembling_start", "NULL");
      CurrentOrder.insert("assembling_end", "NULL");
      if (!Database->updateRecordById("orders", CurrentOrder)) {
        sendLog(QString("Получена ошибка при увеличении количества собранных "
                        "паллет в заказе %1. ")
                    .arg(CurrentOrder.value("id")));
        return DatabaseQueryError;
      }

      if (CurrentOrder.value("assembled_units") ==
          CurrentOrder.value("quantity")) {
        CurrentOrder.insert("ready_indicator", "true");
        if (!Database->updateRecordById("orders", CurrentOrder)) {
          sendLog(QString("Получена ошибка при завершении сборки заказа %1. ")
                      .arg(CurrentOrder.value("id")));
          return DatabaseQueryError;
        }
      }
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releaseBoxManually(
    const QString& id) {
  QHash<QString, QString> transponderRecord;

  while (1) {
    transponderRecord.insert("id", "");
    transponderRecord.insert("box_id", id);
    transponderRecord.insert("release_counter", "0");
    if (!Database->getRecordByPart("transponders", transponderRecord)) {
      sendLog(QString("Получена ошибка при поиске транспондера в боксе %1. ")
                  .arg(transponderRecord.value("box_id")));
      return DatabaseQueryError;
    }

    if (transponderRecord.isEmpty()) {
      sendLog(
          QString("Все транспондеры из бокса %1 были принудительно выпущены. ")
              .arg(transponderRecord.value("box_id")));
      break;
    }

    ReturnStatus ret =
        releaseTransponderManually(transponderRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releasePalletManually(
    const QString& id) {
  QHash<QString, QString> boxRecord;

  while (1) {
    boxRecord.insert("id", "");
    boxRecord.insert("pallet_id", id);
    boxRecord.insert("ready_indicator", "false");
    if (!Database->getRecordByPart("boxes", boxRecord)) {
      sendLog(
          QString("Получена ошибка при поиске бокса в паллете %1. ").arg(id));
      return DatabaseQueryError;
    }

    if (boxRecord.isEmpty()) {
      sendLog(QString("Все боксы в паллете %1 были принудительно выпущены. ")
                  .arg(id));
      break;
    }

    ReturnStatus ret = releaseBoxManually(boxRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::releaseOrderManually(
    const QString& id) {
  QHash<QString, QString> palletRecord;

  while (1) {
    palletRecord.insert("id", "");
    palletRecord.insert("order_id", id);
    palletRecord.insert("ready_indicator", "false");
    if (!Database->getRecordByPart("pallets", palletRecord)) {
      sendLog(
          QString("Получена ошибка при поиске паллеты в заказе %1. ").arg(id));
      return DatabaseQueryError;
    }

    if (palletRecord.isEmpty()) {
      sendLog(QString("Все паллеты в заказе %1 были принудительно выпущены. ")
                  .arg(palletRecord.value("order_id")));
      break;
    }

    ReturnStatus ret = releasePalletManually(palletRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::refundTransponderManually(const QString& id) {
  if (!getCurrentContext(id)) {
    sendLog(QString("Получена ошибка при получении контекста транспондера %1. ")
                .arg(id));
    return DatabaseQueryError;
  }

  CurrentTransponder.insert("release_counter", "0");
  CurrentTransponder.insert("ucid", "NULL");
  CurrentTransponder.insert("awaiting_confirmation", "false");
  if (!Database->updateRecordById("transponders", CurrentTransponder)) {
    sendLog(QString("Получена ошибка при возврате транспондера %1. ").arg(id));
    return DatabaseQueryError;
  }

  if ((CurrentPallet.value("ready_indicator") == "true") &&
      (CurrentOrder.value("assembled_units").toInt() > 0)) {
    if ((CurrentPallet.value("shipped_indicator") == "true") &&
        (CurrentOrder.value("shipped_units").toInt() > 0)) {
      CurrentOrder.insert(
          "shipped_units",
          QString::number(CurrentOrder.value("shipped_units").toInt() - 1));
    }
    CurrentOrder.insert(
        "assembled_units",
        QString::number(CurrentOrder.value("assembled_units").toInt() - 1));
    CurrentOrder.insert("ready_indicator", "false");
    CurrentOrder.insert("fully_shipped", "false");
    CurrentOrder.insert("assembling_start", "NULL");
    CurrentOrder.insert("assembling_end", "NULL");
    if (!Database->updateRecordById("orders", CurrentOrder)) {
      sendLog(QString("Получена ошибка при снятии статуса готовности "
                      "заказа '%1'. ")
                  .arg(CurrentOrder.value("id")));
      return DatabaseQueryError;
    }
  }

  if ((CurrentBox.value("ready_indicator") == "true") &&
      (CurrentPallet.value("assembled_units").toInt() > 0)) {
    CurrentPallet.insert(
        "assembled_units",
        QString::number(CurrentPallet.value("assembled_units").toInt() - 1));
    CurrentPallet.insert("ready_indicator", "false");
    CurrentPallet.insert("shipped_indicator", "false");
    CurrentPallet.insert("assembling_start", "NULL");
    CurrentPallet.insert("assembling_end", "NULL");
    if (!Database->updateRecordById("pallets", CurrentPallet)) {
      sendLog(QString("Получена ошибка при снятии статуса готовности "
                      "паллеты '%1'. ")
                  .arg(CurrentPallet.value("id")));
      return DatabaseQueryError;
    }
  }

  if (CurrentBox.value("assembled_units").toInt() > 0) {
    CurrentBox.insert(
        "assembled_units",
        QString::number(CurrentBox.value("assembled_units").toInt() - 1));
  }
  CurrentBox.insert("ready_indicator", "false");
  CurrentBox.insert("assembling_start", "NULL");
  CurrentBox.insert("assembling_end", "NULL");
  CurrentBox.insert("production_line_id", "NULL");
  if (!Database->updateRecordById("boxes", CurrentBox)) {
    sendLog(QString("Получена ошибка при уменьшении количества собранных "
                    "транспондеров в боксе %1. ")
                .arg(CurrentBox.value("id")));
    return DatabaseQueryError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundBoxManually(
    const QString& id) {
  QHash<QString, QString> transponderRecord;

  while (1) {
    transponderRecord.insert("id", "");
    transponderRecord.insert("box_id", id);
    transponderRecord.insert("release_counter", ">=1");
    if (!Database->getRecordByPart("transponders", transponderRecord)) {
      sendLog(QString("Получена ошибка при поиске транспондера в боксе %1. ")
                  .arg(transponderRecord.value("box_id")));
      return DatabaseQueryError;
    }

    if (transponderRecord.isEmpty()) {
      sendLog(
          QString("Все транспондеры из бокса %1 были возвращены. ").arg(id));
      break;
    }

    ReturnStatus ret = refundTransponderManually(transponderRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundPalletManually(
    const QString& id) {
  QHash<QString, QString> boxRecord;

  while (1) {
    boxRecord.insert("id", "");
    boxRecord.insert("pallet_id", id);
    boxRecord.insert("assembled_units", ">=1");
    if (!Database->getRecordByPart("boxes", boxRecord)) {
      sendLog(
          QString("Получена ошибка при поиске бокса в паллете %1. ").arg(id));
      return DatabaseQueryError;
    }

    if (boxRecord.isEmpty()) {
      sendLog(QString("Все боксы из паллеты %1 были возвращены. ").arg(id));
      break;
    }

    ReturnStatus ret = refundBoxManually(boxRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::refundOrderManually(
    const QString& id) {
  QHash<QString, QString> palletRecord;

  while (1) {
    palletRecord.insert("id", "");
    palletRecord.insert("order_id", id);
    palletRecord.insert("assembled_units", ">=1");
    if (!Database->getRecordByPart("pallets", palletRecord)) {
      sendLog(
          QString("Получена ошибка при поиске паллеты в заказе %1. ").arg(id));
      return DatabaseQueryError;
    }

    if (palletRecord.isEmpty()) {
      sendLog(QString("Все паллеты из заказа %1 были возвращены. ").arg(id));
      break;
    }

    ReturnStatus ret = refundPalletManually(palletRecord.value("id"));
    if (ret != Completed) {
      return ret;
    }
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::shipTransponder(
    const QString& id,
    QFile* reg) {
  QHash<QString, QString> transponderData;
  ReturnStatus ret = getTransponderData(id, &transponderData);
  if (ret != Completed) {
    sendLog(QString("Получена ошибка при генерации данных транспондера %1.")
                .arg(id));
    return ret;
  }

  QString registerRecord =
      QString("%1;%2;%3;%4;%5\n")
          .arg(transponderData.value("transponder_model"),
               transponderData.value("sn"), transponderData.value("pan"),
               transponderData.value("box_id"),
               transponderData.value("pallet_id"));
  QTextStream out(reg);
  out << registerRecord;
  out.flush();

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::shipPallet(
    const QString& id,
    QFile* reg) {
  QStringList tables;
  QStringList foreignKeys;
  QHash<QString, QString> mergedRecord;
  uint32_t quantity;
  ReturnStatus ret;

  tables.append("transponders");
  tables.append("boxes");
  tables.append("pallets");
  foreignKeys.append("box_id");
  foreignKeys.append("pallet_id");
  mergedRecord.insert("transponders.id", "");
  mergedRecord.insert("pallet_id", id);
  if (!Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
    sendLog(QString("Получена ошибка поиске первого транспондера в паллете %1.")
                .arg(id));
    return DatabaseQueryError;
  }

  if (!getCurrentContext(mergedRecord.value("id"))) {
    sendLog(QString("Получена ошибка при получении контекста транспондера %1. ")
                .arg(mergedRecord.value("id")));
    return DatabaseQueryError;
  }

  quantity = CurrentBox.value("quantity").toUInt() *
             CurrentPallet.value("quantity").toUInt();
  for (uint32_t i = 0; i < quantity; i++) {
    ret = shipTransponder(
        QString::number(mergedRecord.value("id").toUInt() + i), reg);
    if (ret != Completed) {
      sendLog(
          QString(
              "Получена ошибка поиске отгрузке %1 транспондера в паллете %2.")
              .arg(QString::number(mergedRecord.value("id").toUInt() + i), id));
      return ret;
    }
  }

  return Completed;
}
