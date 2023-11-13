#include "database_table_model.h"

DatabaseTableModel::DatabaseTableModel(QObject* parent)
    : QAbstractTableModel(parent) {
  setObjectName("DatabaseTableModel");
}

DatabaseTableModel::~DatabaseTableModel() {
  clear();
}

void DatabaseTableModel::build(
    const QHash<QString, QSharedPointer<QVector<QString>>>* records) {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  beginResetModel();

  // Очищаем старые данные
  deleteAll();

  // Устанавливаем новые данные
  for (QHash<QString, QSharedPointer<QVector<QString>>>::const_iterator it =
           records->constBegin();
       it != records->constEnd(); it++) {
    Headers.append(it.key());
    Data.append(it.value());
  }

  endResetModel();
}

void DatabaseTableModel::clear() {
  // Блокируем доступ
  QMutexLocker locker(&Mutex);

  beginResetModel();

  deleteAll();

  endResetModel();
}

bool DatabaseTableModel::isEmpty() const {
  return ((!Headers.isEmpty())) && (!Data.isEmpty());
}

int DatabaseTableModel::rowCount(const QModelIndex& parent) const {
  if (Data.size() < 1) {
    return 0;
  }

  return Data.first()->size();
}

int DatabaseTableModel::columnCount(const QModelIndex& parent) const {
  return Data.size();
}

QVariant DatabaseTableModel::data(const QModelIndex& index, int role) const {
  if (index.column() > Headers.size())
    return QVariant();

  if (index.row() > Data.size())
    return QVariant();

  if (role == Qt::DisplayRole) {
    return Data.at(index.column())->at(index.row());
  } else
    return QVariant();
}

QVariant DatabaseTableModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const {
  if (section > Headers.size()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return Headers.at(section);
  } else {
    return QVariant();
  }
}

void DatabaseTableModel::deleteAll() {
  Headers.clear();
  Data.clear();
}
