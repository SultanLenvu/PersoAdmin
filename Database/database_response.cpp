#include "database_response.h"

DatabaseRecordTable::DatabaseRecordTable(QObject* parent) : QObject(parent) {}

DatabaseRecordTable::~DatabaseRecordTable() {}

void DatabaseRecordTable::addField(const QString& name,
                                const QSharedPointer<QVector<QString>> values) {
  Records.insert(name, values);
}

void DatabaseRecordTable::addField(const QString& name,
                                uint32_t recordNumber,
                                bool init) {
  QSharedPointer<QVector<QString>> values(new QVector<QString>());
  if (init) {
    values->resize(recordNumber);
  } else {
    values->reserve(recordNumber);
  }

  Records.insert(name, values);
}

void DatabaseRecordTable::addFieldValue(const QString& field,
                                     const QString& value) {
  Records.value(field)->append(value);
}

uint32_t DatabaseRecordTable::getRecordNumber() const {
  if (Records.size() < 1) {
    return 0;
  }

  return Records.constBegin().value()->size();
}

uint32_t DatabaseRecordTable::getFieldNumber() const {
  return Records.size();
}

bool DatabaseRecordTable::isEmpty() {
  if (Records.size() > 0) {
    return false;
  } else {
    return true;
  }
}

QString DatabaseRecordTable::getValue(uint32_t record, const QString& field) {
  if (!Records.contains(field)) {
    return QString();
  }

  if (Records.value(field)->size() < record) {
    return QString();
  }

  return Records.value(field)->at(record);
}

void DatabaseRecordTable::clear() {
  Records.clear();
}
