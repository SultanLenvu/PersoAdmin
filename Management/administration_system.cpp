#include "administration_system.h"

AdministrationSystem::AdministrationSystem(QObject* parent) : QObject(parent) {
  setObjectName("AdministrationSystem");
  loadSettings();
  qRegisterMetaType<ReturnStatus>("ReturnStatus");

  createDatabaseController();
}

void AdministrationSystem::applySettings() {
  emit logging("Применение новых настроек. ");
  loadSettings();
  Database->applySettings();
}

AdministrationSystem::ReturnStatus AdministrationSystem::connectDatabase() {
  emit logging("Подключение к базе данных. ");
  if (!Database->connect()) {
    emit logging("Не удалось установить соединение с базой данных. ");
    return DatabaseConnectionError;
  }

  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::disconnectDatabase() {
  emit logging("Отключение от базы данных. ");
  Database->disconnect();

  return Completed;
}

void AdministrationSystem::createDatabaseController() {
  Database = new PostgresController(this, "AdministratorConnection");
  connect(Database, &IDatabaseController::logging, this,
          &AdministrationSystem::proxyLogging);
}

AdministrationSystem::ReturnStatus AdministrationSystem::clearDatabaseTable(
    const QString& tableName) {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->clearTable(tableName)) {
    emit logging(
        QString("Получена ошибка при очистке таблицы %1. ").arg(tableName));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging(QString("Таблица %1 очищена. ").arg(tableName));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getDatabaseTable(
    const QString& tableName,
    DatabaseTableModel* buffer) {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->getTable(tableName, 10, buffer)) {
    emit logging(QString("Получена ошибка при получении данных из таблицы %1. ")
                     .arg(tableName));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging(
      QString("Данные из таблицы %1 успешно получены. ").arg(tableName));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::getCustomResponse(
    const QString& req,
    DatabaseTableModel* buffer) {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!Database->execCustomRequest(req, buffer)) {
    emit logging("Получена ошибка при выполнении кастомного запроса. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Кастомный запрос успешно выполнен. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::createNewOrder(
    const QMap<QString, QString>* orderParameters) {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  emit logging("Добавление заказа. ");
  if (!addOrder(orderParameters)) {
    emit logging("Получена ошибка при добавлении заказа. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Добавление палет. ");
  if (!addPallets(orderParameters)) {
    emit logging("Получена ошибка при добавлении палет. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Добавление боксов. ");
  if (!addBoxes(orderParameters)) {
    emit logging("Получена ошибка при добавлении боксов. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Добавление транспондеров. ");
  if (!addTransponders(orderParameters)) {
    emit logging("Получена ошибка при добавлении транспондеров. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Новый заказ успешно создан. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::startOrderAssembling(
    const QString& orderId) {
  QMap<QString, QString> boxRecord;
  QMap<QString, QString> productionLineRecord;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
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
      emit logging(QString("Получена ошибка при поиске неактивной "
                           "производственной линии '%1'. ")
                       .arg(productionLineRecord.value("login")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если незадействованная линия не найдена
    if (productionLineRecord.isEmpty()) {
      emit logging(QString("Все производственные линии активны. "));
      break;
    }

    // Ищем бокс для производственной линии
    if (!searchBoxForProductionLine(orderId, productionLineRecord.value("id"),
                                    boxRecord)) {
      emit logging(QString("Получена ошибка при запуске сборки бокса %1. ")
                       .arg(boxRecord.value("id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если свободных боксов не найдено
    if (boxRecord.isEmpty()) {
      emit logging(QString("В заказе %1 заняты все боксы. ").arg(orderId));
      break;
    }

    // Запускаем сборку бокса
    if (!startBoxProcessing(boxRecord.value("id"),
                            productionLineRecord.value("id"))) {
      emit logging(QString("Получена ошибка при запуске сборки бокса %1. ")
                       .arg(boxRecord.value("id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    boxRecord.clear();
    productionLineRecord.clear();
  }

  emit logging(QString("Сборка заказа %1 запущена. ").arg(orderId));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::stopOrderAssembling(
    const QString& orderId) {
  QMap<QString, QString> conditions;
  QMap<QString, QString> newValues;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    Database->abortTransaction();
    return DatabaseTransactionError;
  }

  emit logging("Деактивация линий производства. ");
  conditions.insert("active", "true");
  newValues.insert("active", "false");
  newValues.insert("transponder_id", "NULL");
  if (!Database->updateAllRecordsByPart("production_lines", conditions,
                                        newValues)) {
    emit logging(QString("Получена ошибка при остановке линий производства. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging("Остановка процесса сборки всех боксов. ");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("boxes", conditions, newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки всех боксов. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging("Остановка процесса сборки всех паллет.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("pallets", conditions, newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки всех паллет."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging(QString("Остановка процесса сборки заказа %1.").arg(orderId));
  newValues.insert("id", orderId);
  newValues.insert("in_process", "false");
  if (!Database->updateRecordById("orders", newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки заказа %1.")
                     .arg(orderId));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging(QString("Сборка заказа %1 остановлена. ").arg(orderId));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::deleteLastOrder() {
  QMap<QString, QString> conditions;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  emit logging("Удаление последнего добавленного заказа. ");
  conditions.insert("in_process", "false");
  conditions.insert("ready_indicator", "false");
  Database->removeLastRecordByCondition("orders", conditions);

  emit logging("Последний добавленный заказ успешно удален. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::createNewProductionLine(
    const QMap<QString, QString>* productionLineParameters) {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Добавляем линию производства
  emit logging("Добавление линии производства. ");
  if (!addProductionLine(productionLineParameters)) {
    emit logging("Получена ошибка при добавлении производственной линии. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Новая линия производства успешно создана. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::deleteLastProductionLine() {
  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  if (!removeLastProductionLine()) {
    emit logging("Получена ошибка при удалении последней линии производства. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Последняя линия производства успешно удалена. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::linkProductionLineWithBox(
    const QMap<QString, QString>* linkParameters) {
  QMap<QString, QString> productionLineRecord;
  QMap<QString, QString> newBoxRecord;
  QMap<QString, QString> transponderRecord;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Запрашиваем данные о производственной линии
  productionLineRecord.insert("login", linkParameters->value("login"));
  productionLineRecord.insert("password", linkParameters->value("password"));
  productionLineRecord.insert("transponder_id", "");
  productionLineRecord.insert("id", "");
  if (!Database->getRecordByPart("production_lines", productionLineRecord)) {
    emit logging(
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
    emit logging(QString("Получена ошибка при поиске данных бокса %1. ")
                     .arg(linkParameters->value("box_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  // Если бокс не найден, то связывание недопустимо
  if (newBoxRecord.isEmpty()) {
    emit logging(QString("Бокс %1 не существует, связывание с "
                         "производственной линией %2 невозможно. ")
                     .arg(linkParameters->value("box_id"),
                          productionLineRecord.value("id")));
    return LogicError;
  }

  // Если бокс уже собран, то связывание недопустимо
  if (newBoxRecord.value("ready_indicator") == "true") {
    emit logging(QString("Бокс %1 уже собран, связывание с "
                         "производственной линией %2 невозможно. ")
                     .arg(linkParameters->value("box_id"),
                          productionLineRecord.value("id")));
    return LogicError;
  }
  // Если бокс занят другой производственной линией, то связывание
  // недопустимо
  if (newBoxRecord.value("in_process") == "true") {
    emit logging(QString("Бокс %1 уже в процессе сборки, связывание с "
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
      emit logging(
          QString("Получена ошибка при поиске данных транспондера %1. ")
              .arg(productionLineRecord.value("transponder_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Останавливаем сборку соответсвующего бокса
    if (!stopBoxProcessing(transponderRecord.value("box_id"))) {
      emit logging(QString("Получена ошибка при остановке сборки бокса %1. ")
                       .arg(transponderRecord.value("box_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }
  }

  // Запускаем процесс сборки бокса
  if (!startBoxProcessing(newBoxRecord.value("id"),
                          productionLineRecord.value("id"))) {
    emit logging(QString("Получена ошибка при запуске сборки бокса %1. ")
                     .arg(newBoxRecord.value("id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging(
      QString("Линия производства %1 успешно связана с боксом %2. ")
          .arg(productionLineRecord.value("id"), newBoxRecord.value("id")));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::shutdownAllProductionLines() {
  QMap<QString, QString> conditions;
  QMap<QString, QString> newValues;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  emit logging("Деактивация линий производства. ");
  conditions.insert("active", "true");
  newValues.insert("active", "false");
  newValues.insert("transponder_id", "NULL");
  if (!Database->updateAllRecordsByPart("production_lines", conditions,
                                        newValues)) {
    emit logging(QString("Получена ошибка при остановке линий производства. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging("Остановка процесса сборки всех боксов. ");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("boxes", conditions, newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки всех боксов. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging("Остановка процесса сборки всех паллет.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("pallets", conditions, newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки всех паллет."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging("Остановка процесса сборки всех заказов.");
  conditions.insert("in_process", "true");
  newValues.insert("in_process", "false");
  if (!Database->updateAllRecordsByPart("orders", conditions, newValues)) {
    emit logging(QString("Получена ошибка при остановке сборки всех заказов."));
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  newValues.clear();
  conditions.clear();

  emit logging(QString("Все линии производства остановлены. "));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::allocateInactiveProductionLines(const QString& orderId) {
  QMap<QString, QString> productionLineRecord;
  QMap<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
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
      emit logging(QString("Получена ошибка при поиске неактивной "
                           "производственной линии '%1'. ")
                       .arg(productionLineRecord.value("login")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Если незадействованная линия не найдена
    if (productionLineRecord.isEmpty()) {
      emit logging(QString("Все производственные линии активны. "));
      if (!Database->closeTransaction()) {
        emit logging("Получена ошибка при закрытии транзакции. ");
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
      emit logging(
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
      emit logging(
          QString("Получена ошибка при активации производственной линии "
                  "производственной линии %1. ")
              .arg(productionLineRecord.value("if")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    // Запускаем сборку бокса
    if (!startBoxProcessing(mergedRecord.value("box_id"),
                            productionLineRecord.value("id"))) {
      emit logging(QString("Получена ошибка при запуске сборки бокса %1. ")
                       .arg(mergedRecord.value("box_id")));
      Database->abortTransaction();
      return DatabaseQueryError;
    }

    productionLineRecord.clear();
    mergedRecord.clear();
    foreignKeys.clear();
    tables.clear();
  }

  emit logging(QString("Все производственные линии активны. "));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus AdministrationSystem::initIssuerTable() {
  QMap<QString, QString> record;
  int32_t lastId = 0;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  // Получаем идентифкатор последнего добавленного заказа
  record.insert("id", "");
  if (!Database->getLastRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  lastId = record.value("id").toInt();

  emit logging("Инициализация таблицы эмитентов. ");
  record.insert("id", QString::number(++lastId));
  record.insert("name", "Пауэр Синтез");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Автодор");
  record.insert("efc_context_mark", "570002FF0070");
  if (!Database->addRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Новое качество дорог");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Западный скоростной диаметр");
  record.insert("efc_context_mark", "570001FF0070");
  if (!Database->addRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  record.insert("id", QString::number(++lastId));
  record.insert("name", "Объединенные системы сбора платы");
  record.insert("efc_context_mark", "000000000001");
  if (!Database->addRecord("issuers", record)) {
    emit logging("Получена ошибка при выполнении запроса в базу данных. ");
    Database->abortTransaction();
    return DatabaseQueryError;
  }
  record.clear();

  emit logging("Таблица эмитентов успешно инициализирована. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::initTransportMasterKeysTable() {
  QMap<QString, QString> transportKeysRecord;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  transportKeysRecord.insert("id", "");
  if (!Database->getLastRecord("transport_master_keys", transportKeysRecord)) {
    emit logging("Получена ошибка при поиске последнего заказа. ");
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
    emit logging(
        QString("Получена ошибка при добавлении транспортных мастер ключей. "));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging("Таблица транспортных ключей успешно инициализирована. ");
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

AdministrationSystem::ReturnStatus
AdministrationSystem::linkIssuerWithMasterKeys(
    const QMap<QString, QString>* linkParameters) {
  QMap<QString, QString> issuerRecord;

  if (!Database->openTransaction()) {
    emit logging("Получена ошибка при открытиеи транзакции. ");
    return DatabaseTransactionError;
  }

  emit logging(QString("Обновление записи эмитента %1.")
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
    emit logging(QString("Получена ошибка при обновлении записи эмитента %1.")
                     .arg(linkParameters->value("issuer_id")));
    Database->abortTransaction();
    return DatabaseQueryError;
  }

  emit logging(QString("Эмитент %1 успешно связан с мастер ключами %2. ")
                   .arg(linkParameters->value("issuer_id"),
                        linkParameters->value("master_keys_id")));
  if (!Database->closeTransaction()) {
    emit logging("Получена ошибка при закрытии транзакции. ");
    return DatabaseTransactionError;
  }
  return Completed;
}

void AdministrationSystem::loadSettings() {}

bool AdministrationSystem::addOrder(
    const QMap<QString, QString>* orderParameters) const {
  QMap<QString, QString> issuerRecord;
  QMap<QString, QString> orderRecord;
  int32_t lastId = 0;

  issuerRecord.insert("id", "");
  issuerRecord.insert("name", orderParameters->value("issuer_name"));
  if (!Database->getRecordByPart("issuers", issuerRecord)) {
    emit logging(QString("Не найден идентифкатор эмитента \"%1\".")
                     .arg(orderParameters->value("IssuerName")));
    return false;
  }

  orderRecord.insert("id", "");
  if (!Database->getLastRecord("orders", orderRecord)) {
    emit logging("Получена ошибка при поиске последнего заказа. ");
    return false;
  }
  lastId = orderRecord.value("id").toInt();

  // Формируем новую запись
  orderRecord.insert("id", QString::number(lastId + 1));
  orderRecord.insert("issuer_id", issuerRecord.value("id"));
  orderRecord.insert("capacity", "0");
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
    emit logging("Получена ошибка при добавлении заказа. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::addPallets(
    const QMap<QString, QString>* orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t orderCapacity = transponderCount / (palletCapacity * boxCapacity);
  QMap<QString, QString> orderRecord;
  QMap<QString, QString> palletRecord;
  int32_t lastId = 0;

  // Поиск идентификатора незаполненного заказа
  orderRecord.insert("id", "");
  orderRecord.insert("capacity", "0");
  if (!Database->getRecordByPart("orders", orderRecord)) {
    emit logging(
        "Получена ошибка при поиске идентификатора незаполненного заказа. ");
    return false;
  }

  // Заполняем заказ
  for (uint32_t i = 0; i < orderCapacity; i++) {
    // Получаем идентификатор последней палеты
    palletRecord.insert("id", "");
    if (!Database->getLastRecord("pallets", palletRecord)) {
      emit logging("Получена ошибка при поиске последней палеты. ");
      return false;
    }
    lastId = palletRecord.value("id").toInt();

    // Формируем новую запись
    palletRecord.insert("id", QString::number(lastId + 1));
    palletRecord.insert("capacity", "0");
    palletRecord.insert("order_id", orderRecord.value("id"));
    // Добавляем новую запись
    if (!Database->addRecord("pallets", palletRecord)) {
      emit logging("Получена ошибка при добавлении палеты. ");
      return false;
    }
  }

  // Заполнение заказа
  orderRecord.insert("capacity", QString::number(orderCapacity));
  if (!Database->updateRecordById("orders", orderRecord)) {
    emit logging(QString("Получена ошибка при заполнении заказа %1. ")
                     .arg(orderRecord.value("id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::addBoxes(
    const QMap<QString, QString>* orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t palletCapacity = orderParameters->value("pallet_capacity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t palletCount = transponderCount / (palletCapacity * boxCapacity);
  QMap<QString, QString> palletRecord;
  QMap<QString, QString> boxRecord;
  int32_t lastId = 0;

  for (uint32_t i = 0; i < palletCount; i++) {
    // Получаем идентификатор незаполненной палеты
    palletRecord.insert("id", "");
    palletRecord.insert("capacity", "0");
    if (!Database->getRecordByPart("pallets", palletRecord)) {
      emit logging(
          "Получена ошибка при поиске идентификатора незаполненной палеты. ");
      return false;
    }

    // Создаем боксы
    for (uint32_t i = 0; i < palletCapacity; i++) {
      // Получаем идентификатор последнего добавленного бокса
      boxRecord.insert("id", "");
      if (!Database->getLastRecord("boxes", boxRecord)) {
        emit logging("Получена ошибка при поиске последнего бокса. ");
        return false;
      }
      lastId = boxRecord.value("id").toInt();

      // Формируем новую запись
      boxRecord.insert("id", QString::number(lastId + 1));
      boxRecord.insert("capacity", "0");
      boxRecord.insert("pallet_id", palletRecord.value("id"));
      // Добавляем новую запись
      if (!Database->addRecord("boxes", boxRecord)) {
        emit logging("Получена ошибка при добавлении бокса. ");
        return false;
      }
    }

    // Заполнение палеты
    palletRecord.insert("capacity", QString::number(palletCapacity));
    if (!Database->updateRecordById("pallets", palletRecord)) {
      emit logging(QString("Получена ошибка при заполнении палеты %1. ")
                       .arg(palletRecord.value("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::addTransponders(
    const QMap<QString, QString>* orderParameters) const {
  uint32_t transponderCount =
      orderParameters->value("transponder_quantity").toInt();
  uint32_t boxCapacity = orderParameters->value("box_capacity").toInt();
  uint32_t boxCount = transponderCount / boxCapacity;
  QMap<QString, QString> boxRecord;
  QMap<QString, QString> transponderRecord;
  int32_t lastId = 0;

  QFile panFile(orderParameters->value("pan_file_path"));
  if (!panFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit logging("Получена ошибка при открытии PAN-файла. ");
    return false;
  }
  QTextStream in(&panFile);

  for (uint32_t i = 0; i < boxCount; i++) {
    // Получаем идентификатор незаполненного бокса
    boxRecord.insert("id", "");
    boxRecord.insert("capacity", "0");
    if (!Database->getRecordByPart("boxes", boxRecord)) {
      emit logging(
          "Получена ошибка при поиске идентификатора незаполненного бокса. ");
      return false;
    }

    // Создаем транспондеры
    for (uint32_t i = 0; i < boxCapacity; i++) {
      // Получаем идентификатор последнего добавленного транспондера
      transponderRecord.insert("id", "");
      if (!Database->getLastRecord("transponders", transponderRecord)) {
        emit logging("Получена ошибка при поиске последнего бокса. ");
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
        emit logging("Получена ошибка при добавлении транспондера. ");
        return false;
      }
    }

    // Заполнение бокса
    boxRecord.insert("capacity", QString::number(boxCapacity));
    if (!Database->updateRecordById("boxes", boxRecord)) {
      emit logging(QString("Получена ошибка при заполнении бокса %1. ")
                       .arg(boxRecord.value("id")));
      return false;
    }
  }

  return true;
}

bool AdministrationSystem::addProductionLine(
    const QMap<QString, QString>* productionLineParameters) const {
  QMap<QString, QString> productionLineRecord;
  QMap<QString, QString> mergedRecord;
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
    emit logging(
        "Получена ошибка при поиске первого транспондера свободного бокса. ");
    return false;
  }

  // Получаем идентификатор последней линии производства
  productionLineRecord.insert("id", "");
  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
    emit logging(
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
    emit logging("Получена ошибка при добавлении линии производства. ");
    return false;
  }

  return true;
}

bool AdministrationSystem::startBoxProcessing(
    const QString& id,
    const QString& productionLineId) const {
  QMap<QString, QString> boxRecord;
  QMap<QString, QString> productionLineRecord;
  QMap<QString, QString> transponderRecord;

  // Получаем данные о боксе
  boxRecord.insert("id", id);
  boxRecord.insert("pallet_id", "");
  boxRecord.insert("in_process", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    emit logging("Получена ошибка при поиске данных о боксе. ");
    return false;
  }

  // Ищем в боксе первый невыпущенный транспондер в боксе
  transponderRecord.insert("id", "");
  transponderRecord.insert("release_counter", "0");
  transponderRecord.insert("box_id", id);
  if (!Database->getRecordByPart("transponders", transponderRecord)) {
    emit logging(QString("Получена ошибка при поиске "
                         "невыпущенного транспондера в боксе %1. ")
                     .arg(id));
    return false;
  }

  // Связываем транспондер с производственной линией
  productionLineRecord.insert("id", productionLineId);
  productionLineRecord.insert("transponder_id", transponderRecord.value("id"));
  productionLineRecord.insert("active", "true");
  if (!Database->updateRecordById("production_lines", productionLineRecord)) {
    emit logging(
        QString("Получена ошибка при связывании производственной линии %1 с "
                "транспондером %2. ")
            .arg(productionLineRecord.value("id"),
                 transponderRecord.value("id")));
    return false;
  }

  // Связываем бокс с производственной линией и обновляем время начала сборки
  if (boxRecord.value("in_process") != "true") {
    boxRecord.insert("in_process", "true");
    boxRecord.insert("production_line_id", productionLineId);
    if (!Database->updateRecordById("boxes", boxRecord)) {
      emit logging(
          QString("Получена ошибка при связывании производственной линии %1 с "
                  "боксом %2. ")
              .arg(productionLineId, id));
      return false;
    }

    // Запуск сборки палеты
    if (!startPalletProcessing(boxRecord.value("pallet_id"))) {
      emit logging(QString("Не удалось запустить сборку палеты %1. ")
                       .arg(boxRecord.value("pallet_id")));
      return false;
    }
  } else {
    emit logging(
        QString("Бокс %1 уже в процессе сборки.").arg(boxRecord.value("id")));
  }
  return true;
}

bool AdministrationSystem::startPalletProcessing(const QString& id) const {
  QMap<QString, QString> palletRecord;

  palletRecord.insert("id", id);
  palletRecord.insert("order_id", "");
  palletRecord.insert("in_process", "");
  if (!Database->getRecordById("pallets", palletRecord)) {
    emit logging("Получена ошибка при поиске данных о паллете. ");
    return false;
  }

  if (palletRecord.value("in_process") != "true") {
    palletRecord.insert("in_process", "true");
    if (!Database->updateRecordById("pallets", palletRecord)) {
      emit logging("Получена ошибка при запуске сборки палеты. ");
      return false;
    }

    // Запуск сборки заказа
    if (!startOrderProcessing(palletRecord.value("order_id"))) {
      emit logging(QString("Не удалось запустить сборку заказа. ")
                       .arg(palletRecord.value("order_id")));
      return false;
    }
  } else {
    emit logging(QString("Палета %1 уже в процессе сборки. ")
                     .arg(palletRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::startOrderProcessing(const QString& id) const {
  QMap<QString, QString> orderRecord;

  orderRecord.insert("id", id);
  if (!Database->getRecordById("orders", orderRecord)) {
    emit logging("Получена ошибка при поиске данных о заказе. ");
    return false;
  }

  if (orderRecord.value("in_process") != "true") {
    orderRecord.insert("in_process", "true");
    orderRecord.insert(
        "assembling_start",
        QDateTime::currentDateTime().toString(TIMESTAMP_TEMPLATE));
    if (!Database->updateRecordById("orders", orderRecord)) {
      emit logging("Получена ошибка при запуске сборки заказа. ");
      return false;
    }
  } else {
    emit logging(QString("Заказ %1 уже в процессе сборки. ")
                     .arg(orderRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::removeLastProductionLine() const {
  QMap<QString, QString> productionLineRecord;
  QMap<QString, QString> boxRecord;

  // Получение последней добавленной линии производства
  productionLineRecord.insert("id", "");
  if (!Database->getLastRecord("production_lines", productionLineRecord)) {
    emit logging(
        "Получена ошибка при поиске последней добавленной производственной "
        "линии. ");
    return false;
  }

  // Получение бокса, связанного с последней добавленной линией производства
  boxRecord.insert("id", "");
  boxRecord.insert("production_line_id", productionLineRecord.value("id"));
  if (!Database->getRecordByPart("boxes", boxRecord)) {
    emit logging(
        "Получена ошибка при поиске бокса, связанного с последней "
        "добавленной линией производства. ");
    return false;
  }

  // Удаляем запись из таблицы линий производства
  if (!Database->removeLastRecordById("production_lines")) {
    emit logging("Получена ошибка при удалении последней линии производства. ");
    return false;
  }

  if (!stopBoxProcessing(boxRecord.value("id"))) {
    emit logging(QString("Получена ошибка при остановке сборки бокса %1. ")
                     .arg(boxRecord.value("id")));
    return false;
  }

  return true;
}

bool AdministrationSystem::stopBoxProcessing(const QString& id) const {
  QMap<QString, QString> boxRecord;
  QMap<QString, QString> mergedRecord;
  QMap<QString, QString> productionLineRecord;
  QStringList tables;
  QStringList foreignKeys;

  // Получаем данные о боксе
  boxRecord.insert("id", id);
  boxRecord.insert("pallet_id", "");
  boxRecord.insert("in_process", "");
  boxRecord.insert("production_line_id", "");
  if (!Database->getRecordById("boxes", boxRecord)) {
    emit logging("Получена ошибка при поиске данных о боксе. ");
    return false;
  }

  // Останавливаем производственную линию, связанную с боксом
  productionLineRecord.insert("id", boxRecord.value("production_line_id"));
  productionLineRecord.insert("active", "false");
  productionLineRecord.insert("transponder_id", "NULL");
  if (!Database->updateRecordById("production_lines", productionLineRecord)) {
    emit logging(
        QString("Получена ошибка при остановке производственной линии %1.")
            .arg(productionLineRecord.value("id")));
    return false;
  }

  if (boxRecord.value("in_process") != "false") {
    boxRecord.insert("in_process", "false");
    if (!Database->updateRecordById("boxes", boxRecord)) {
      emit logging("Получена ошибка при остановке сборки бокса. ");
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
        emit logging(QString("Остановка сборки паллеты %1. ")
                         .arg(boxRecord.value("pallet_id")));
        if (!stopPalletProcessing(boxRecord.value("pallet_id"))) {
          emit logging("Не удалось остановить сборку палеты. ");
          return false;
        }
      } else {
        emit logging(
            "В паллете еще есть боксы, находящиеся в процессе сборки. ");
      }
    }
  } else {
    emit logging(QString("Бокс %1 не находится в процессе сборки. ")
                     .arg(boxRecord.value("id")));
  }
  return true;
}

bool AdministrationSystem::stopPalletProcessing(const QString& id) const {
  QMap<QString, QString> palletRecord;
  QMap<QString, QString> mergedRecord;
  QStringList tables;
  QStringList foreignKeys;

  palletRecord.insert("id", id);
  palletRecord.insert("order_id", "");
  palletRecord.insert("in_process", "");
  if (!Database->getRecordById("pallets", palletRecord)) {
    emit logging("Получена ошибка при поиске данных о паллете. ");
    return false;
  }

  if (palletRecord.value("in_process") != "false") {
    palletRecord.insert("in_process", "false");
    if (!Database->updateRecordById("pallets", palletRecord)) {
      emit logging("Получена ошибка при остановке сборки палеты. ");
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
        emit logging(QString("Остановка сборки заказа %1. ")
                         .arg(palletRecord.value("order_id")));
        if (!stopOrderProcessing(palletRecord.value("order_id"))) {
          emit logging("Не удалось остановить сборку заказа. ");
          return false;
        }
      } else {
        emit logging(
            "В заказе еще есть паллеты, находящиеся в процессе сборки. ");
      }
    }
  } else {
    emit logging(QString("Палета %1 не находится в процессе сборки. ")
                     .arg(palletRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::stopOrderProcessing(const QString& id) const {
  QMap<QString, QString> orderRecord;

  orderRecord.insert("id", id);
  if (!Database->getRecordById("orders", orderRecord)) {
    emit logging("Получена ошибка при поиске данных о заказе. ");
    return false;
  }

  if (orderRecord.value("in_process") != "false") {
    orderRecord.insert("in_process", "false");
    if (!Database->updateRecordById("orders", orderRecord)) {
      emit logging("Получена ошибка при остановке сборки заказа. ");
      return false;
    }
  } else {
    emit logging(QString("Заказ %1 не находится в процессе сборки. ")
                     .arg(orderRecord.value("id")));
  }

  return true;
}

bool AdministrationSystem::searchBoxForProductionLine(
    const QString& orderId,
    const QString& productionLineId,
    QMap<QString, QString>& boxRecord) const {
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
    emit logging(
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
      emit logging(
          QString("Получена ошибка при поиске свободного бокса в заказе %1. ")
              .arg(orderId));
      return false;
    }
  }

  return true;
}

void AdministrationSystem::proxyLogging(const QString& log) const {
  if (sender()->objectName() == "IDatabaseController") {
    emit logging("Database controller - " + log);
  } else {
    emit logging("Unknown - " + log);
  }
}
