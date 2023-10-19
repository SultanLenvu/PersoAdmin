#include "administration_system.h"

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
  if (!Database->connect()) {
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
  record.insert("name", "Западный скоростной диаметр");
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
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getTransponderData(
    const QString& id,
    QHash<QString, QString>* data) {
  QHash<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;

  // Запрашиваем атрибуты
  tables.append("transponders");
  tables.append("boxes");
  tables.append("pallets");
  tables.append("orders");
  tables.append("issuers");
  foreignKeys.append("box_id");
  foreignKeys.append("pallet_id");
  foreignKeys.append("order_id");
  foreignKeys.append("issuer_id");

  mergedRecord.insert("manufacturer_id", "");
  mergedRecord.insert("equipment_class", "");
  mergedRecord.insert("transponder_model", "");
  mergedRecord.insert("accr_reference", "");
  mergedRecord.insert("ucid", "");

  mergedRecord.insert("efc_context_mark", "");
  mergedRecord.insert("personal_account_number", "");

  mergedRecord.insert("box_id", "");
  mergedRecord.insert("pallet_id", "");
  mergedRecord.insert("order_id", "");
  mergedRecord.insert("issuer_id", "");
  mergedRecord.insert("issuers.name", "");
  mergedRecord.insert("transponders.id", id);

  if (!Database->getMergedRecordByPart(tables, foreignKeys, mergedRecord)) {
    return DatabaseQueryError;
  }

  // Данные переносимые без изменений
  data->insert("box_id", mergedRecord.value("box_id"));
  data->insert("pallet_id", mergedRecord.value("pallet_id"));
  data->insert("order_id", mergedRecord.value("order_id"));

  // Удаляем пробелы из названия модели
  QString tempModel = mergedRecord.value("transponder_model");
  data->insert("transponder_model", tempModel.remove(" "));

  // Преобразуем в десятичный формат
  QString manufacturerId =
      QString::number(mergedRecord.value("manufacturer_id").toInt(nullptr, 16));

  // Дата сборки
  QDate date = QDate::currentDate();
  QString batteryInsertationDate =
      QString("%1%2")
          .arg(QString::number(date.weekNumber()), 2, QChar('0'))
          .arg(QString::number(date.year() % 100), 2, QChar('0'));

  // Дополняем серийник до 10 цифр нулями слева
  QString extendedTransponderId =
      QString("%1").arg(mergedRecord.value("id"), 10, QChar('0'));

  // Конструируем серийный номер транспондера
  data->insert("sn",
               QString("%1%2%3").arg(manufacturerId, batteryInsertationDate,
                                     extendedTransponderId));

  // Вычленяем символ F из personal_account_number
  QString tempPan = mergedRecord.value("personal_account_number");
  data->insert("pan", tempPan.remove(QChar('F')));

  // Название компании-заказчика
  data->insert("issuer_name", mergedRecord.value("name"));

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
    return DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponderRecord.value("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponderRecord.value("id")));
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
    return DatabaseQueryError;
  }

  // Запрашиваем данные транспондера
  if (!getTransponderData(transponderRecord.value("id"), &transponderData)) {
    emit logging(
        QString("Получена ошибка при получении данных транспондера %1. ")
            .arg(transponderRecord.value("id")));
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
    return DatabaseQueryError;
  }

  orderRecord.insert("id", palletRecord.value("order_id"));
  orderRecord.insert("transponder_model", "");
  if (!Database->getRecordById("orders", orderRecord)) {
    emit logging(QString("Получена ошибка при поиске заказа с id %1. ")
                     .arg(palletRecord.value("order_id")));
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
    return DatabaseQueryError;
  }

  // Сохраняем идентификатор последнего бокса
  data->insert("last_box_id", boxRecord.value("id"));
  uint32_t totalQuantity = palletRecord.value("assembled_units").toInt() *
                           boxRecord.value("quantity").toInt();
  data->insert("quantity", QString::number(totalQuantity));

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::releaseTranspondersManually(const QString& table,
                                                  const QString& id) {
  if (table == "transponders") {
    if (!releaseTransponderManually(id)) {
      sendLog(QString("Не удалось принудительно выпустить транспондер %1.")
                  .arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "boxes") {
    if (!releaseBoxManually(id)) {
      sendLog(QString("Не удалось принудительно выпустить бокс %1.").arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "pallets") {
    if (!releasePalletManually(id)) {
      sendLog(
          QString("Не удалось принудительно выпустить паллету %1.").arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "orders") {
    if (!releaseOrderManually(id)) {
      sendLog(QString("Не удалось принудительно выпустить заказ %1.").arg(id));
      return DatabaseQueryError;
    }
  } else {
    sendLog("Получено неизвестное имя таблицы. ");
    return ParameterError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::refundTranspondersManually(const QString& table,
                                                 const QString& id) {
  if (table == "transponders") {
    if (!refundTransponderManually(id)) {
      sendLog(
          QString("Не удалось принудительно вернуть транспондер %1.").arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "boxes") {
    if (!refundBoxManually(id)) {
      sendLog(QString("Не удалось принудительно вернуть бокс %1.").arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "pallets") {
    if (!refundPalletManually(id)) {
      sendLog(QString("Не удалось принудительно вернуть паллету %1.").arg(id));
      return DatabaseQueryError;
    }
  } else if (table == "orders") {
    if (!refundOrderManually(id)) {
      sendLog(QString("Не удалось принудительно вернуть заказ %1.").arg(id));
      return DatabaseQueryError;
    }
  } else {
    sendLog("Получено неизвестное имя таблицы. ");
    return ParameterError;
  }

  return Completed;
}

void AdministrationSystem::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("log_system/global_enable").toBool();
}

void AdministrationSystem::sendLog(const QString& log) const {
  if (LogEnable) {
    emit logging(QString("%1 - %2").arg(objectName(), log));
  }
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
  orderRecord.insert("transponder_model",
                     orderParameters->value("transponder_model"));
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
  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
    sendLog(
        "Получена ошибка при поиске последней добавленной производственной "
        "линии. ");
    return false;
  }

  // Получение бокса, связанного с последней добавленной линией производства
  boxRecord.insert("id", "");
  boxRecord.insert("production_line_id", productionLineRecord.value("id"));
  if (!Database->getRecordByPart("boxes", boxRecord)) {
    sendLog(
        "Получена ошибка при поиске бокса, связанного с последней "
        "добавленной линией производства. ");
    return false;
  }

  // Удаляем запись из таблицы линий производства
  if (!Database->removeLastRecordById("production_lines")) {
    sendLog("Получена ошибка при удалении последней линии производства. ");
    return false;
  }

  if (!stopBoxProcessing(boxRecord.value("id"))) {
    sendLog(QString("Получена ошибка при остановке сборки бокса %1. ")
                .arg(boxRecord.value("id")));
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

bool AdministrationSystem::releaseTransponderManually(const QString& id) {
  QHash<QString, QString> transponderRecord;
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> palletRecord;
  QHash<QString, QString> orderRecord;

  transponderRecord.insert("id", id);
  transponderRecord.insert("release_counter", "1");
  transponderRecord.insert("ucid", "NULL");
  transponderRecord.insert("awaiting_confirmation", "false");
  if (!Database->updateRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при обновлении записи транспондера %1. ")
                .arg(id));
    return false;
  }

  transponderRecord.clear();

  transponderRecord.insert("id", id);
  transponderRecord.insert("box_id", "");
  if (!Database->getRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при получении записи транспондера %1. ")
                .arg(id));
    return false;
  }

  boxRecord.insert("id", transponderRecord.value("box_id"));
  boxRecord.insert("assembled_units", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    sendLog(QString("Получена ошибка при получении записи транспондера %1. ")
                .arg(id));
    return false;
  }

  return true;
}

bool AdministrationSystem::releaseBoxManually(const QString& id) {
  QHash<QString, QString> conditions;
  QHash<QString, QString> newValues;

  newValues.insert("release_counter", "1");
  newValues.insert("ucid", "NULL");
  newValues.insert("awaiting_confirmation", "false");
  conditions.insert("box_id", id);
  if (!Database->updateAllRecordsByPart("transponders", conditions,
                                        newValues)) {
    sendLog(QString("Получена ошибка при обновлении записей транспондеров, "
                    "содержащихся в боксе %1. ")
                .arg(id));
    return false;
  }

  return true;
}

bool AdministrationSystem::releasePalletManually(const QString& id) {}

bool AdministrationSystem::releaseOrderManually(const QString& id) {}

bool AdministrationSystem::refundTransponderManually(const QString& id) {
  QHash<QString, QString> transponderRecord;
  QHash<QString, QString> boxRecord;

  transponderRecord.insert("id", id);
  transponderRecord.insert("release_counter", "0");
  transponderRecord.insert("ucid", "NULL");
  transponderRecord.insert("awaiting_confirmation", "false");
  if (!Database->updateRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при обновлении записи транспондера %1. ")
                .arg(id));
    return false;
  }

  transponderRecord.insert("box_id", "");
  if (!Database->getRecordById("transponders", transponderRecord)) {
    sendLog(QString("Получена ошибка при обновлении записи транспондера %1. ")
                .arg(id));
    return false;
  }

  boxRecord.insert("id", transponderRecord.value("box_id"));
  boxRecord.insert("assembled_units", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    sendLog(QString("Получена ошибка при обновлении записи бокса %1. ")
                .arg(transponderRecord.value("box_id")));
    return false;
  }

  if (boxRecord.value("assembled_units").toInt() > 0) {
    boxRecord.insert(
        "assembled_units",
        QString::number(boxRecord.value("assembled_units").toInt() - 1));
    if (!Database->updateRecordById("boxes", boxRecord)) {
      sendLog(QString("Получена ошибка при обновлении записи бокса %1. ")
                  .arg(transponderRecord.value("box_id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::refundBoxManually(const QString& id) {
  QHash<QString, QString> conditions;
  QHash<QString, QString> transponderRecord;
  QHash<QString, QString> boxRecord;

  boxRecord.insert("id", id);
  boxRecord.insert("in_process", "false");
  boxRecord.insert("ready_indicator", "false");
  boxRecord.insert("assembling_start", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  boxRecord.insert("assembling_end", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  boxRecord.insert("production_line_id", "NULL");
  boxRecord.insert("assembled_units", "0");
  if (!Database->updateRecordById("boxes", boxRecord)) {
    sendLog(
        QString("Получена ошибка при обновлении записи бокса %1. ").arg(id));
    return false;
  }

  transponderRecord.insert("release_counter", "0");
  transponderRecord.insert("ucid", "NULL");
  transponderRecord.insert("awaiting_confirmation", "false");
  conditions.insert("box_id", id);
  if (!Database->updateAllRecordsByPart("transponders", conditions,
                                        transponderRecord)) {
    sendLog(QString("Получена ошибка при обновлении записей транспондеров, "
                    "содержащихся в боксе %1. ")
                .arg(id));
    return false;
  }

  return true;
}

bool AdministrationSystem::refundPalletManually(const QString& id) {
  QHash<QString, QString> boxRecord;
  QHash<QString, QString> palletRecord;

  palletRecord.insert("id", id);
  palletRecord.insert("in_process", "false");
  palletRecord.insert("ready_indicator", "false");
  palletRecord.insert("assembling_start", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  palletRecord.insert("assembling_end", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  palletRecord.insert("shipped_indicator", "false");
  palletRecord.insert("assembled_units", "0");
  if (!Database->updateRecordById("pallets", palletRecord)) {
    sendLog(
        QString("Получена ошибка при обновлении записи паллеты %1. ").arg(id));
    return false;
  }

  while (1) {
    // Ищем собранные боксы
    boxRecord.insert("id", "");
    boxRecord.insert("ready_indicator", "true");
    if (!Database->getRecordByPart("boxes", boxRecord)) {
      sendLog(
          QString("Получена ошибка при получении записи собранного бокса. "));
      return false;
    }

    // Если собранных боксов не нашлось
    if (boxRecord.isEmpty()) {
      // Ищем бокс, который в процессе сборки
      boxRecord.insert("id", "");
      boxRecord.insert("in_process", "true");
      if (!Database->getRecordByPart("boxes", boxRecord)) {
        sendLog(
            QString("Получена ошибка при получении записи бокса, "
                    "находящегося в процессе сборки. "));
        return false;
      }
      // Если и таковой не нашлось, то выходим
      if (boxRecord.isEmpty()) {
        break;
      }
    }

    // Осуществляем возврат бокса
    if (!refundBoxManually(boxRecord.value("id"))) {
      sendLog(QString("Получена ошибка при возврате бокса %1. ")
                  .arg(boxRecord.value("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::refundOrderManually(const QString& id) {
  QHash<QString, QString> palletRecord;
  QHash<QString, QString> orderRecord;

  orderRecord.insert("id", id);
  orderRecord.insert("in_process", "false");
  orderRecord.insert("ready_indicator", "false");
  orderRecord.insert("assembling_start", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  orderRecord.insert("assembling_end", POSTGRES_TIMESTAMP_DEFAULT_VALUE);
  orderRecord.insert("shipped_units", "0");
  orderRecord.insert("fully_shipped", "false");
  orderRecord.insert("assembled_units", "0");
  if (!Database->updateRecordById("orders", orderRecord)) {
    sendLog(
        QString("Получена ошибка при обновлении записи заказа %1. ").arg(id));
    return false;
  }

  while (1) {
    // Ищем собранные палеты
    palletRecord.insert("id", "");
    palletRecord.insert("ready_indicator", "true");
    if (!Database->getRecordByPart("pallets", palletRecord)) {
      sendLog(
          QString("Получена ошибка при получении записи собранной паллеты. "));
      return false;
    }

    // Если собранных паллет не нашлось
    if (palletRecord.isEmpty()) {
      // Ищем паллету, которая в процессе сборки
      palletRecord.insert("id", "");
      palletRecord.insert("in_process", "true");
      if (!Database->getRecordByPart("pallets", palletRecord)) {
        sendLog(
            QString("Получена ошибка при получении записи паллеты, "
                    "находящейся в процессе сборки. "));
        return false;
      }
      // Если и таковой не нашлось, то выходим
      if (palletRecord.isEmpty()) {
        break;
      }
    }

    // Осуществляем возврат паллеты
    if (!refundPalletManually(palletRecord.value("id"))) {
      sendLog(QString("Получена ошибка при возврате паллеты %1. ")
                  .arg(palletRecord.value("id")));
      return false;
    }
  }

  return true;
}
