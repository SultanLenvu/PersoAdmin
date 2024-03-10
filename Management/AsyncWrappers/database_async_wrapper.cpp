#include "database_async_wrapper.h"
#include "postgre_sql_database.h"
#include "sql_query_values.h"

DatabaseAsyncWrapper::DatabaseAsyncWrapper(const QString& name)
    : NamedObject(name),
      LoggableObject(name),
      Database(new PostgreSqlDatabase("PostgreSqlDatabase")) {}

DatabaseAsyncWrapper::~DatabaseAsyncWrapper() {}

std::shared_ptr<AbstractSqlDatabase> DatabaseAsyncWrapper::database() {
  return Database;
}

void DatabaseAsyncWrapper::connect() {
  initOperation("connect");

  if (!Database->connect()) {
    processOperationError("connect", ReturnStatus::DatabaseConnectionError);
    return;
  }

  completeOperation("connect");
}

void DatabaseAsyncWrapper::disconnect() {
  initOperation("connect");

  Database->disconnect();

  completeOperation("connect");
}

void DatabaseAsyncWrapper::getTable(const QString& name) {
  initOperation("getTable");

  std::shared_ptr<SqlQueryValues> response(new SqlQueryValues());

  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(0);

  if (!Database->readRecords(name, *response)) {
    processOperationError("getTable", ReturnStatus::DatabaseConnectionError);
    return;
  }

  emit responseReady(response);
  completeOperation("getTable");
}

void DatabaseAsyncWrapper::execCustomRequest(const QString& req) {
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

void DatabaseAsyncWrapper::getTransponderData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getTransponderData");

  std::shared_ptr<StringDictionary> data(new StringDictionary());

  if (!generateTransponderData((*param)["id"], *data)) {
    processOperationError("getTransponderData",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  emit dataReady(data);
  completeOperation("getTransponderData");
}

void DatabaseAsyncWrapper::getBoxData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getBoxData");

  std::shared_ptr<StringDictionary> data(new StringDictionary());

  if (!generateBoxData((*param)["id"], *data)) {
    processOperationError("getBoxData", ReturnStatus::DatabaseTransactionError);
    return;
  }

  emit dataReady(data);
  completeOperation("getBoxData");
}

void DatabaseAsyncWrapper::getPalletData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getPalletData");

  std::shared_ptr<StringDictionary> data(new StringDictionary());

  if (!generatePalletData((*param)["id"], *data)) {
    processOperationError("getPalletData",
                          ReturnStatus::DatabaseTransactionError);
    return;
  }

  emit dataReady(data);
  completeOperation("getPalletData");
}

bool DatabaseAsyncWrapper::generateTransponderData(const QString& id,
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
  if (!Database->readMergedRecords(
          tables, QString("transponders.id = %1").arg(id), record)) {
    return false;
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

  return true;
}

bool DatabaseAsyncWrapper::generateBoxData(const QString& id,
                                           StringDictionary& data) {
  SqlQueryValues box;
  SqlQueryValues transponders;
  StringDictionary transponderData;

  if (!Database->readRecords("boxes", QString("id = %1").arg(id), box)) {
    return false;
  }

  // Сохраняем данные бокса
  data.insert("id", id);
  data.insert("assembled_units", box.get("assembled_units"));

  // Ищем транспондеры в боксе
  Database->setCurrentOrder(Qt::AscendingOrder);
  Database->setRecordMaxCount(box.get("assembled_units").toUInt());
  if (!Database->readRecords("transponders", QString("box_id = %1").arg(id),
                             transponders)) {
    return false;
  }

  // Запрашиваем данные транспондера
  if (!generateTransponderData(transponders.get("id"), transponderData)) {
    return false;
  }

  // Сохраняем серийник первого транспондера в боксе
  data.insert("first_transponder_sn", transponderData.value("sn"));
  transponderData.clear();

  // Запрашиваем данные транспондера
  if (!generateTransponderData(transponders.getLast("id"), transponderData)) {
    return false;
  }

  // Сохраняем серийник последнего транспондера в боксе
  data.insert("last_transponder_sn", transponderData.value("sn"));

  // Сохраняем модель транспондера
  QString modelTemp = transponderData.value("transponder_model");
  data.insert("transponder_model", modelTemp.remove(' '));

  return true;
}

bool DatabaseAsyncWrapper::generatePalletData(const QString& id,
                                              StringDictionary& data) {
  SqlQueryValues boxes;
  SqlQueryValues pallet;
  SqlQueryValues order;

  if (!Database->readRecords("pallets", QString("id = %1").arg(id), pallet)) {
    return false;
  }

  if (!Database->readRecords(
          "orders", QString("id = %1").arg(pallet.get("order_id")), order)) {
    return false;
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
  if (!Database->readRecords("boxes", QString("pallet_id = %1").arg(id),
                             boxes)) {
    return false;
  }

  // Сохраняем идентификатор первого бокса
  data.insert("first_box_id", boxes.get("id"));

  // Сохраняем идентификатор последнего бокса
  data.insert("last_box_id", boxes.getLast("id"));
  uint32_t totalQuantity =
      pallet.get("assembled_units").toInt() * boxes.get("quantity").toInt();

  data.insert("quantity", QString::number(totalQuantity));

  return true;
}
