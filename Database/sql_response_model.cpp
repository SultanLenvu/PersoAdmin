#include "sql_response_model.h"

#include <QSqlRecord>

SqlResponseModel::SqlResponseModel(QObject* parent)
    : QAbstractTableModel(parent) {
  setObjectName("SqlResponseModel");
}

SqlResponseModel::~SqlResponseModel() {}

bool SqlResponseModel::isEmpty() const {
  if (Values.size() > 0) {
    return false;
  } else {
    return true;
  }
}

QString SqlResponseModel::getValue(uint32_t record,
                                   const QString& field) const {
  return Values.at(FieldPosition.value(field))->at(record);
}

QString SqlResponseModel::getFirstValue(const QString& field) const {
  return Values.at(FieldPosition.value(field))->at(0);
}

void SqlResponseModel::extractRecords(QSqlQuery& request) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  beginResetModel();
  clear();

  QSqlRecord rec = request.record();
  for (int32_t i = 0; i < rec.count(); i++) {
    QSharedPointer<QVector<QString>> values(new QVector<QString>);
    values->reserve(request.size());
    Values.append(values);
    Fields.append(rec.fieldName(i));
    FieldPosition.insert(rec.fieldName(i), i);
  }

  request.setForwardOnly(true);
  while (request.next()) {
    for (int32_t i = 0; i < request.record().count(); i++) {
      Values.at(request.at())->append(request.value(i).toString());
    }
  }

  endResetModel();
}

uint32_t SqlResponseModel::getRecordNumber() const {
  return Values.size();
}

uint32_t SqlResponseModel::getFieldNumber() const {
  return Fields.size();
}

void SqlResponseModel::clear() {
  Values.clear();
  Fields.clear();
}

int SqlResponseModel::rowCount(const QModelIndex& parent) const {
  return Values.size();
}

int SqlResponseModel::columnCount(const QModelIndex& parent) const {
  return Fields.size();
}

QVariant SqlResponseModel::data(const QModelIndex& index, int role) const {
  if (index.column() > Fields.size())
    return QVariant();

  if (index.row() > Values.size())
    return QVariant();

  if (role == Qt::DisplayRole) {
    return Values.at(index.column())->at(index.row());
  } else
    return QVariant();
}

QVariant SqlResponseModel::headerData(int section,
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
