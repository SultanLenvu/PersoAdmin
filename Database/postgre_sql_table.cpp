#include "postgre_sql_table.h"

PostgreSqlTable::PostgreSqlTable(const QString& name,
                                 const QString& connectionName)
    : AbstractSqlTable(nullptr) {
  setObjectName(name);

  ConnectionName = connectionName;
}

PostgreSqlTable::~PostgreSqlTable() {}

QString PostgreSqlTable::getPrimaryKey() const {
  return PrimaryKey;
}

void PostgreSqlTable::setPrimaryKey(const QString& newPrimaryKey) {
  PrimaryKey = newPrimaryKey;
}

uint32_t PostgreSqlTable::getRecordCount() const {
  return RecordCount;
}

void PostgreSqlTable::setRecordCount(uint32_t newRecordCount) {
  RecordCount = newRecordCount;
}

QVector<QString>* PostgreSqlTable::columns() {
  return &Columns;
}

QHash<QString, QString>* PostgreSqlTable::relations() {
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
    return false;
  }
  while (request.next()) {
    Relations.insert(request.value(1).toString(), request.value(0).toString());
  }

  return true;
}

bool PostgreSqlTable::createRecords(
    QVector<QSharedPointer<QHash<QString, QString> > >& records) const {
  // Создаем запрос
  QString requestText = QString("INSERT INTO public.%1 (").arg(objectName());
  //  for (QHash<QString, QString>::const_iterator it = record.constBegin();
  //       it != record.constEnd(); it++) {
  //    requestText += QString("%1, ").arg(it.key());
  //  }
  //  requestText.chop(2);
  //  requestText += ") VALUES (";
  //  for (QHash<QString, QString>::const_iterator it = record.constBegin();
  //       it != record.constEnd(); it++) {
  //    if (it.value() != "NULL") {
  //      requestText += QString("'%1', ").arg(it.value());
  //    } else {
  //      requestText += QString("%1, ").arg(it.value());
  //    }
  //  }
  //  requestText.chop(2);
  //  requestText += ");";

  //  // Выполняем запрос
  //  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  //  if (request.exec(requestText)) {
  //    sendLog("Запись добавлена. ");
  //    return true;
  //  } else {  // Обработка ошибки выполнения запроса
  //    sendLog(request.lastError().text());
  //    sendLog("Отправленный запрос: " + requestText);
  //    return false;
  //  }
  return true;
}

bool PostgreSqlTable::readRecords(
    const QHash<QString, QString>& searchValues,
    QVector<QSharedPointer<QHash<QString, QString> > >& records) const {
  return true;
}

bool PostgreSqlTable::updateRecords(const QString& condition,
                                    QHash<QString, QString>& newValues) const {
  return true;
}

bool PostgreSqlTable::deleteRecords(const QString& condition) const {
  return true;
}

bool PostgreSqlTable::clearTable() const {
  return true;
}
