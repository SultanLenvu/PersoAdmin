#include "sql_table.h"

SqlTable::SqlTable(const QString& name) {
  Name = name;
}

SqlTable::~SqlTable() {}

SqlTable::SqlTable(const SqlTable& other) {
  Name = other.Name;
  PrimaryKey = other.PrimaryKey;
  Columns = other.Columns;
  Relations = other.Relations;
  RecordCount = other.RecordCount;
}

SqlTable& SqlTable::operator=(const SqlTable& other) {
  if (this != &other) {
    Name = other.Name;
    PrimaryKey = other.PrimaryKey;
    Columns = other.Columns;
    Relations = other.Relations;
    RecordCount = other.RecordCount;
  }
  return *this;
}

SqlTable::SqlTable(SqlTable&& other) noexcept {
  Name = std::move(other.Name);
  PrimaryKey = std::move(other.PrimaryKey);
  Columns = std::move(other.Columns);
  Relations = std::move(other.Relations);
  RecordCount = other.RecordCount;

  other.RecordCount = 0;
}

SqlTable& SqlTable::operator=(SqlTable&& other) noexcept {
  if (this != &other) {
    Name = std::move(other.Name);
    PrimaryKey = std::move(other.PrimaryKey);
    Columns = std::move(other.Columns);
    Relations = std::move(other.Relations);
    RecordCount = std::move(other.RecordCount);
  }
  return *this;
}

QString SqlTable::getName() const {
  return Name;
}

void SqlTable::setName(const QString& newName) {
  Name = newName;
}

QString SqlTable::getPrimaryKey() const {
  return PrimaryKey;
}

void SqlTable::setPrimaryKey(const QString& newPrimaryKey) {
  PrimaryKey = newPrimaryKey;
}

uint32_t SqlTable::getRecordCount() const {
  return RecordCount;
}

void SqlTable::setRecordCount(uint32_t newRecordCount) {
  RecordCount = newRecordCount;
}

QVector<QString>* SqlTable::columns() {
  return &Columns;
}

QHash<QString, QString>* SqlTable::relations() {
  return &Relations;
}

bool SqlTable::init(QSqlDatabase* db) {
  if (!db->isOpen()) {
    return false;
  }

  QSqlQuery request(*db);
  QSqlRecord record = db->record(Name);

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
                        .arg(Name))) {
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
              .arg(Name))) {
    return false;
  }
  while (request.next()) {
    Relations.insert(request.value(1).toString(), request.value(0).toString());
  }

  return true;
}
