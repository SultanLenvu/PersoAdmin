#include "hash_table_model.h"

void HashTableModel::setData(const StringDictionary& data) {
  beginResetModel();

  Data = data;

  // Настраиваем конвертер
  Converter.clear();
  uint32_t i = 0;
  for (auto itb = data.constBegin(), ite = data.constEnd(); itb != ite; ++itb) {
    Converter[i++] = itb.key();
  }

  endResetModel();
}

void HashTableModel::setMatchTable(const StringDictionary& match) {
  MatchTable = match;
}

void HashTableModel::clear() {
  beginResetModel();

  Converter.clear();
  Data.clear();

  endResetModel();
}

bool HashTableModel::isEmpty() const {
  return false;
}

int HashTableModel::columnCount(const QModelIndex& parent) const {
  return 1;
}

int HashTableModel::rowCount(const QModelIndex& parent) const {
  return Data.size();
}

QVariant HashTableModel::data(const QModelIndex& index, int role) const {
  if (index.column() > 1) {
    return QVariant();
  }

  if (index.row() >= Data.size()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  return Data.value(Converter[index.row()]);
}

QVariant HashTableModel::headerData(int section,
                                Qt::Orientation orientation,
                                int role) const {
  if (section >= Converter.size()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return "Значение";
  }

  if (orientation == Qt::Vertical) {
    return QVariant();
  }

  if (!MatchTable.isEmpty()) {
    return MatchTable.value(Converter[section]);
  }

  return Converter[section];
}
