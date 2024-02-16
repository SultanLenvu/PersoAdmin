#include <QSqlRecord>

#include "sql_query_values.h"

SqlQueryValues::SqlQueryValues() {}

SqlQueryValues::~SqlQueryValues() {}

SqlQueryValues::SqlQueryValues(const SqlQueryValues& other)
    : Fields(other.Fields),
      FieldIndex(other.FieldIndex),
      Values(other.Values) {}

SqlQueryValues::SqlQueryValues(SqlQueryValues&& other) noexcept
    : Fields(std::move(other.Fields)),
      FieldIndex(std::move(other.FieldIndex)),
      Values(std::move(other.Values)) {}

SqlQueryValues& SqlQueryValues::operator=(const SqlQueryValues& other) {
  if (this != &other) {
    Fields = other.Fields;
    FieldIndex = other.FieldIndex;
    Values = other.Values;
  }
  return *this;
}

SqlQueryValues& SqlQueryValues::operator=(SqlQueryValues&& other) noexcept {
  if (this != &other) {
    Fields = std::move(other.Fields);
    FieldIndex = std::move(other.FieldIndex);
    Values = std::move(other.Values);
  }
  return *this;
}

QString SqlQueryValues::fieldName(uint32_t i) const {
  return Fields.at(i);
}

QString SqlQueryValues::get(uint32_t record, const QString& field) const {
  return Values.at(FieldIndex.value(field))->at(record);
}

QString SqlQueryValues::get(uint32_t record, uint32_t field) const {
  return Values.at(field)->at(record);
}

QString SqlQueryValues::get(uint32_t field) const {
  return Values.at(field)->at(0);
}

QString SqlQueryValues::get(const QString& field) const {
  return Values.at(FieldIndex.value(field))->at(0);
}

QString SqlQueryValues::getLast(const QString& field) const {
  return Values.at(FieldIndex.value(field))->last();
}

int32_t SqlQueryValues::recordCount() const {
  if (Values.isEmpty()) {
    return 0;
  }
  return Values.first()->size();
}

int32_t SqlQueryValues::fieldCount() const {
  return Fields.size();
}

bool SqlQueryValues::isEmpty() const {
  if (Values.isEmpty()) {
    return true;
  }
  return Values.first()->isEmpty();
}

void SqlQueryValues::appendToInsert(QString& queryText) const {
  for (int32_t i = 0; i < Values.first()->size(); i++) {
    queryText.append("(");
    for (int32_t j = 0; j < Values.size(); j++) {
      if (Values.at(j)->at(i) != "NULL") {
        queryText.append(QString("'%1'").arg(Values.at(j)->at(i)));
      } else {
        queryText.append(QString("NULL"));
      }
      queryText.append(", ");
    }
    queryText.chop(2);
    queryText.append("),\n");
  }
  queryText.chop(2);
}

void SqlQueryValues::extractRecords(QSqlQuery& request) {
  Values.clear();
  Fields.clear();
  FieldIndex.clear();

  QSqlRecord rec = request.record();
  for (int32_t i = 0; i < rec.count(); i++) {
    SharedVector<QString> values(new QVector<QString>());
    values->reserve(request.size());
    Values.append(values);

    Fields.append(rec.fieldName(i));
    FieldIndex.insert(rec.fieldName(i), i);
  }

  while (request.next()) {
    for (int32_t i = 0; i < request.record().count(); i++) {
      Values[i]->append(request.value(i).toString());
    }
  }
}

void SqlQueryValues::add(const StringDictionary& record) {
  for (StringDictionary::const_iterator it = record.constBegin();
       it != record.constEnd(); ++it) {
    add(it.key(), it.value());
  }
}

void SqlQueryValues::add(const QString& field,
                         const std::shared_ptr<QVector<QString>>& values) {
  if (FieldIndex.contains(field)) {
    Values.at(FieldIndex.value(field))->append(*values);
  } else {
    Values.append(values);
    Fields.append(field);
    FieldIndex.insert(field, FieldIndex.size());
  }
}

void SqlQueryValues::add(const QString& field, const QString& value) {
  if (FieldIndex.contains(field)) {
    Values.at(FieldIndex.value(field))->append(value);
  } else {
    std::shared_ptr<QVector<QString>> values(new QVector<QString>);
    values->append(value);
    FieldIndex.insert(field, FieldIndex.size());
    Values.append(values);
    Fields.append(field);
  }
}

void SqlQueryValues::addField(const QString& field) {
  if (!FieldIndex.contains(field)) {
    std::shared_ptr<QVector<QString>> values(new QVector<QString>);
    FieldIndex.insert(field, FieldIndex.size());
    Values.append(values);
    Fields.append(field);
  }
}

void SqlQueryValues::clear() {
  Values.clear();
  Fields.clear();
  FieldIndex.clear();
}
