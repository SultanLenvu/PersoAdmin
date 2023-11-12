#include "postgre_sql_table.h"

PostgreSqlTable::PostgreSqlTable(const QString& name,
                                 const QString& connectionName)
    : AbstractSqlTable(nullptr) {
  setObjectName(name);
  loadSettings();

  ConnectionName = connectionName;
  RecordMaxCount = 256;
  CurrentOrder = "ASC";
}

PostgreSqlTable::~PostgreSqlTable() {}

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

const QHash<QString, QString>* PostgreSqlTable::relations() const {
  return &Relations;
}

bool PostgreSqlTable::init() {
  QSqlDatabase db = QSqlDatabase::database(ConnectionName);
  if (!db.isOpen()) {
    return false;
  }

  QSqlQuery request(db);
  QSqlRecord record = db.record(objectName());

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
                        .arg(objectName()))) {
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
              .arg(objectName()))) {
    sendLog("Получена ошибка при поиске внешних зависимостей.");
    return false;
  }
  while (request.next()) {
    Relations.insert(request.value(1).toString(), request.value(0).toString());
  }

  return true;
}

void PostgreSqlTable::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
}

bool PostgreSqlTable::createRecords(
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  if ((records.size() < 1) || (records.constBegin().value()->size() < 1)) {
    return false;
  }

  if (!checkFieldNames(records)) {
    return false;
  }

  // Создаем запрос
  QString requestText = QString("INSERT INTO public.%1 (").arg(objectName());
  for (QVector<QString>::const_iterator it = Columns.constBegin();
       it != Columns.constEnd(); it++) {
    requestText += QString("%1, ").arg(*it);
  }
  requestText.chop(2);
  requestText += ") VALUES (";
  for (int32_t i = 0; i < records.constBegin().value()->size(); i++) {
    for (QVector<QString>::const_iterator it = Columns.constBegin();
         it != Columns.constEnd(); it++) {
      if (records.contains(*it)) {
        requestText += QString("'%1', ").arg(records.value(*it)->at(i));
      } else {
        requestText += QString("NULL, ");
      }
    }
  }
  requestText.chop(2);
  requestText += ");";

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::readRecords(
    const QString& conditions,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  // Создаем запрос
  QString requestText =
      QString("SELECT * FROM public.%1 WHERE %2 ORDER BY %3 %4 LIMIT %5;")
          .arg(objectName(), conditions, PrimaryKey, CurrentOrder,
               QString::number(RecordMaxCount));

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  extractRecords(request, records);
  return true;
}

bool PostgreSqlTable::readLastRecord(
    QHash<QString, QSharedPointer<QVector<QString>>>& record) const {
  // Создаем запрос
  QString requestText =
      QString("SELECT * FROM public.%1 ORDER BY %2 ASC LIMIT 1;")
          .arg(objectName(), PrimaryKey);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  extractRecords(request, record);
  return true;
}

bool PostgreSqlTable::updateRecords(
    const QString& condition,
    const QHash<QString, QString>& newValues) const {
  if (!checkFieldNames(newValues)) {
    return false;
  }

  // Создаем запрос
  QString requestText = QString("UPDATE public.%1 SET ").arg(objectName());
  for (QHash<QString, QString>::const_iterator it = newValues.constBegin();
       it != newValues.constEnd(); it++) {
    if ((it.value() == "NULL") || (it.value().isEmpty())) {
      requestText += QString("%1 = NULL, ").arg(it.key());
    } else {
      requestText += QString("%1 = '%2', ").arg(it.key(), it.value());
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

bool PostgreSqlTable::deleteRecords(const QString& condition) const {
  // Создаем запрос
  QString requestText =
      QString("DELETE FROM public.%1 WHERE %2;").arg(objectName(), condition);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlTable::clear() const {
  // Создаем запрос
  QString requestText = QString("DELETE FROM public.%1;").arg(objectName());

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

void PostgreSqlTable::sendLog(const QString& log) const {
  if (LogEnable) {
    emit const_cast<PostgreSqlTable*>(this)->logging(log);
  }
}

void PostgreSqlTable::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("log_system/global_enable").toBool();
}

bool PostgreSqlTable::checkFieldNames(
    const QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  for (QHash<QString, QSharedPointer<QVector<QString>>>::const_iterator it =
           records.constBegin();
       it != records.constEnd(); it++) {
    if (!Columns.contains(it.key())) {
      return false;
    }
  }

  return true;
}

bool PostgreSqlTable::checkFieldNames(
    const QHash<QString, QString>& record) const {
  for (QHash<QString, QString>::const_iterator it = record.constBegin();
       it != record.constEnd(); it++) {
    if (!Columns.contains(it.key())) {
      return false;
    }
  }

  return true;
}

void PostgreSqlTable::extractRecords(
    QSqlQuery& request,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  QSharedPointer<QVector<QString>> record;
  records.clear();

  for (int32_t i = 0; i < request.record().count(); i++) {
    QSharedPointer<QVector<QString>> record(new QVector<QString>());
    record->resize(request.size());
    records.insert(request.record().fieldName(i), record);
  }

  while (request.next()) {
    for (int32_t i = 0; i < request.record().count(); i++) {
      records.value(request.record().fieldName(i))
          ->append(request.value(i).toString());
    }
  }
}
