#include "postgre_sql_table.h"

PostgreSqlTable::PostgreSqlTable(const QString& name,
                                 const QString& tableName,
                                 const QString& connectionName)
    : NamedObject(name),
      LoggableObject(name),
      TableName(tableName),
      ConnectionName(connectionName),
      RecordMaxCount(1000),
      CurrentOrder("ASC") {}

PostgreSqlTable::~PostgreSqlTable() {}


bool PostgreSqlTable::init() {
  QSqlDatabase db = QSqlDatabase::database(ConnectionName);
  if (!db.isOpen()) {
    return false;
  }

  QSqlQuery request(db);
  QSqlRecord record = db.record(TableName);

  // Получаем названия колонок
  for (int32_t i = 0; i < record.count(); i++) {
    Columns.append(record.fieldName(i));
  }

  // Ищем первичный ключ таблицы
  if (!request.exec(QString("SELECT attname AS column_name "
                            "FROM pg_constraint AS con "
                            "JOIN pg_attribute AS att ON att.attnum = "
                            "ANY(con.conkey) AND att.attrelid = con.conrelid "
                            "WHERE "
                            "con.contype = 'p' "
                            "AND con.conrelid = '%1'::regclass;")
                        .arg(TableName))) {
    sendLog("Получена ошибка при поиске первичного ключа.");
    return false;
  }
  request.next();
  PrimaryKey = request.record().value(0).toString();

  // Ищем зависимости таблицы
  if (!request.exec(
          QString("SELECT "
                  "attname AS column_name, "
                  "confrelid::regclass AS referenced_table "
                  "FROM pg_constraint AS con "
                  "JOIN pg_attribute AS att ON att.attnum = ANY(con.conkey) "
                  "AND att.attrelid = con.conrelid "
                  "JOIN pg_class AS rel ON con.confrelid = rel.oid "
                  "WHERE "
                  "con.contype = 'f' "
                  "AND con.conrelid = '%1'::regclass;")
              .arg(TableName))) {
    sendLog("Получена ошибка при поиске внешних зависимостей.");
    return false;
  }
  while (request.next()) {
    Relations.insert(request.value(1).toString(), request.value(0).toString());
  }

  return true;
}

bool PostgreSqlTable::createRecords(const SqlQueryValues& records) {
  // Создаем запрос
  QString requestText = QString("INSERT INTO public.%1 (").arg(TableName);
  for (int32_t i = 0; i < records.fieldCount(); i++) {
    requestText += QString("%1, ").arg(records.fieldName(i));
  }
  requestText.chop(2);

  requestText += QString(") VALUES ");
  records.appendToInsert(requestText);
  requestText += QString(";");

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::readRecords(SqlQueryValues& response) {
  // Создаем запрос
  QString requestText = QString("SELECT * FROM public.%1 ORDER BY %2 %3 ")
                            .arg(TableName, PrimaryKey, CurrentOrder);
  if (RecordMaxCount > 0) {
    requestText += QString("LIMIT %1").arg(QString::number(RecordMaxCount));
  }
  requestText += ";";

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  response.extractRecords(request);
  return true;
}

bool PostgreSqlTable::readRecords(const QString& conditions,
                                  SqlQueryValues& response) {
  // Создаем запрос
  QString requestText =
      QString("SELECT * FROM public.%1 WHERE %2 ORDER BY %3 %4 ")
          .arg(TableName, conditions, PrimaryKey, CurrentOrder);
  if (RecordMaxCount > 0) {
    requestText += QString("LIMIT %1").arg(QString::number(RecordMaxCount));
  }
  requestText += ";";

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  response.extractRecords(request);
  return true;
}

bool PostgreSqlTable::readLastRecord(SqlQueryValues& response) {
  // Создаем запрос
  QString requestText =
      QString("SELECT * FROM public.%1 ORDER BY %2 DESC LIMIT 1;")
          .arg(TableName, PrimaryKey);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  response.extractRecords(request);
  return true;
}

bool PostgreSqlTable::updateRecords(const SqlQueryValues& newValues) {
  QString requestText = QString("UPDATE public.%1 SET ").arg(TableName);
  for (int32_t i = 0; i < newValues.fieldCount(); i++) {
    if (newValues.get(i) == "NULL") {
      requestText += QString("%1 = NULL, ").arg(newValues.fieldName(i));
    } else {
      requestText +=
          QString("%1 = '%2', ").arg(newValues.fieldName(i), newValues.get(i));
    }
  }
  requestText.chop(2);
  requestText += ";";

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::updateRecords(const QString& condition,
                                    const SqlQueryValues& newValues) {
  QString requestText = QString("UPDATE public.%1 SET ").arg(TableName);
  for (int32_t i = 0; i < newValues.fieldCount(); i++) {
    if (newValues.get(i) == "NULL") {
      requestText += QString("%1 = NULL, ").arg(newValues.fieldName(i));
    } else {
      requestText +=
          QString("%1 = '%2', ").arg(newValues.fieldName(i), newValues.get(i));
    }
  }

  requestText.chop(2);
  requestText += QString(" WHERE %1;").arg(condition);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::deleteRecords(const QString& condition) {
  // Создаем запрос
  QString requestText =
      QString("DELETE FROM public.%1 WHERE %2;").arg(TableName, condition);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::clear() {
  // Создаем запрос
  QString requestText = QString("DELETE FROM public.%1;").arg(TableName);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::getRecordCount(uint32_t& count) {
  // Создаем запрос
  QString requestText =
      QString("SELECT COUNT(*) FROM public.%1;").arg(TableName);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  request.next();
  count = request.value(0).toUInt();

  return true;
}

QString PostgreSqlTable::getPrimaryKey() const {
  return PrimaryKey;
}

void PostgreSqlTable::setPrimaryKey(const QString& newPrimaryKey) {
  PrimaryKey = newPrimaryKey;
}

uint32_t PostgreSqlTable::getRecordMaxCount() const {
  return RecordMaxCount;
}

void PostgreSqlTable::setRecordMaxCount(uint32_t newRecordMaxCount) {
  RecordMaxCount = newRecordMaxCount;
}

Qt::SortOrder PostgreSqlTable::getCurrentOrder() const {
  if (CurrentOrder == "ASC") {
    return Qt::AscendingOrder;
  }

  return Qt::DescendingOrder;
}

void PostgreSqlTable::setCurrentOrder(Qt::SortOrder order) {
  if (order == Qt::AscendingOrder) {
    CurrentOrder = "ASC";
    return;
  }

  CurrentOrder = "DESC";
}

const QVector<QString>* PostgreSqlTable::columns() const {
  return &Columns;
}

const StringDictionary& PostgreSqlTable::relations() const {
  return Relations;
}

bool PostgreSqlTable::checkFieldNames(const SqlQueryValues& records) const {
  for (int32_t i = 0; i < records.fieldCount(); i++) {
    if (!Columns.contains(records.fieldName(i))) {
      return false;
    }
  }

  return true;
}

bool PostgreSqlTable::checkFieldNames(
    const StringDictionary& record) const {
  for (StringDictionary::const_iterator it = record.constBegin();
       it != record.constEnd(); it++) {
    if (!Columns.contains(it.key())) {
      return false;
    }
  }

  return true;
}
