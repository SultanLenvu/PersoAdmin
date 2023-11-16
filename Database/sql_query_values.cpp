#include <QSqlRecord>

#include "sql_query_values.h"

SqlQueryValues::SqlQueryValues(QObject* parent) : QAbstractTableModel{parent} {}

SqlQueryValues::~SqlQueryValues() {}

QString SqlQueryValues::fieldName(uint32_t i) const {
  return Fields.at(i);
}

QString SqlQueryValues::get(uint32_t record, const QString& field) const {
  return Values->at(record)->at(FieldIndex.value(field));
}

QString SqlQueryValues::get(uint32_t record, uint32_t field) const {
  return Values->at(record)->at(field);
}

QString SqlQueryValues::get(const QString& field) const {
  return Values->at(0)->at(FieldIndex.value(field));
}

QString SqlQueryValues::getLast(const QString& field) const {
  return Values->last()->at(FieldIndex.value(field));
}

int32_t SqlQueryValues::recordCount() const {
  return Values->size();
}

int32_t SqlQueryValues::fieldCount() const {
  return Fields.size();
}

bool SqlQueryValues::isEmpty() const {
  return Values->size();
}

void SqlQueryValues::appendToInsert(QString& queryText) const {
  for (int32_t i = 0; i < Values->size(); i++) {
    queryText.append("(");
    for (int32_t j = 0; j < Values->at(i)->size(); j++) {
      queryText.append(Values->at(i)->at(j));
    }
    queryText.append("),\n");
  }
  queryText.chop(2);
}

void SqlQueryValues::extractRecords(QSqlQuery& request) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  beginResetModel();
  clear();

  QSqlRecord rec = request.record();
  for (int32_t i = 0; i < rec.count(); i++) {
    QSharedPointer<QVector<QString>> values(new QVector<QString>);
    values->reserve(request.size());
    Values->append(values);
    Fields.append(rec.fieldName(i));
    FieldIndex.insert(rec.fieldName(i), i);
  }

  request.setForwardOnly(true);
  while (request.next()) {
    for (int32_t i = 0; i < request.record().count(); i++) {
      Values->at(request.at())->append(request.value(i).toString());
    }
  }

  endResetModel();
}

void SqlQueryValues::add(const QHash<QString, QString>& record) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  for (QHash<QString, QString>::const_iterator it = record.constBegin();
       it != record.constEnd(); it++) {
    add(it.key(), it.value());
  }
}

void SqlQueryValues::add(const QString& field,
                         const QSharedPointer<QVector<QString>>& values) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  if (FieldIndex.contains(field)) {
    Values->at(FieldIndex.value(field))->append(*values);
  } else {
    Values->append(values);
    Fields.append(field);
    FieldIndex.insert(field, FieldIndex.size());
  }
}

void SqlQueryValues::add(const QString& field, const QString& value) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  if (FieldIndex.contains(field)) {
    Values->at(FieldIndex.value(field))->append(value);
  } else {
    QSharedPointer<QVector<QString>> values(new QVector<QString>);
    values->append(value);
    FieldIndex.insert(field, FieldIndex.size());
    Values->append(values);
    Fields.append(field);
  }
}

void SqlQueryValues::add(const QString& field) {
  if (!FieldIndex.contains(field)) {
    QSharedPointer<QVector<QString>> values(new QVector<QString>);
    FieldIndex.insert(field, FieldIndex.size());
    Values->append(values);
    Fields.append(field);
  }
}

void SqlQueryValues::clear() {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  Values->clear();
  Fields.clear();
}

int SqlQueryValues::rowCount(const QModelIndex& parent) const {
  return Values->size();
}

int SqlQueryValues::columnCount(const QModelIndex& parent) const {
  return Fields.size();
}

QVariant SqlQueryValues::data(const QModelIndex& index, int role) const {
  if (index.column() > Fields.size())
    return QVariant();

  if (index.row() > Values->size())
    return QVariant();

  if (role == Qt::DisplayRole) {
    return Values->at(index.column())->at(index.row());
  } else
    return QVariant();
}

QVariant SqlQueryValues::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const {
  if (section > Fields.size()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return Fields.at(section);
  } else {
    return QVariant();
  }
}
