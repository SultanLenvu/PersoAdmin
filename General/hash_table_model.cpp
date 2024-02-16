#include "hash_table_model.h"

HashTableModel::HashTableModel() : QAbstractTableModel(nullptr) {}

HashTableModel::~HashTableModel() {}

void HashTableModel::setData(std::shared_ptr<StringDictionary> data) {
  beginResetModel();

  Data = data;

  // Настраиваем конвертер
  Converter.clear();
  uint32_t i = 0;
  for (auto itb = data->constBegin(), ite = data->constEnd(); itb != ite;
       ++itb) {
    Converter[i++] = itb.key();
  }

  endResetModel();
}

void HashTableModel::setMatchTable(std::shared_ptr<StringDictionary> match) {
  MatchTable = match;
}

void HashTableModel::clear() {
  beginResetModel();

  Converter.clear();
  Data.reset();

  endResetModel();
}

bool HashTableModel::isEmpty() const {
  if (!Data) {
    return true;
  }

  return false;
}

int HashTableModel::columnCount(const QModelIndex& parent) const {
  if (!Data) {
    return 0;
  }

  return 1;
}

int HashTableModel::rowCount(const QModelIndex& parent) const {
  if (!Data) {
    return 0;
  }

  return Data->size();
}

QVariant HashTableModel::data(const QModelIndex& index, int role) const {
  if (index.column() > 1) {
    return QVariant();
  }

  if (index.row() >= Data->size()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  return Data->value(Converter.at(index.row()));
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

  if (MatchTable) {
    return MatchTable->value(Converter.at(section));
  }

  return Converter.at(section);
}
