#include "hash_table_model.h"

HashTableModel::HashTableModel() : QAbstractTableModel(nullptr) {}

HashTableModel::~HashTableModel() {}

void HashTableModel::setData(const StringDictionary& table) {
  beginResetModel();

  // Очищаем старые данные
  Values.clear();
  Headers.clear();

  // Устанавливаем новые данные
  for (QHash<QString, QString>::const_iterator it1 = table.constBegin();
       it1 != table.constEnd(); it1++) {
    Values.append(it1.value());
    Headers.append(MatchTable->value(it1.key()));
  }

  endResetModel();
}

void HashTableModel::setMatchTable(std::shared_ptr<StringDictionary> match) {
  MatchTable = match;
}

void HashTableModel::clear() {
  beginResetModel();

  Values.clear();
  Headers.clear();

  endResetModel();
}

bool HashTableModel::isEmpty() const {
  return Values.isEmpty();
}

int HashTableModel::columnCount(const QModelIndex& parent) const {
  if (Values.isEmpty()) {
    return 0;
  }

  return 1;
}

int HashTableModel::rowCount(const QModelIndex& parent) const {
  return Values.size();
}

QVariant HashTableModel::data(const QModelIndex& index, int role) const {
  if (index.column() > 1) {
    return QVariant();
  }

  if (index.row() > (Values.size())) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    return Values.at(index.row());
  } else
    return QVariant();
}

QVariant HashTableModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const {
  if (section > (Values.size())) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return "Значение";
  }

  if (orientation == Qt::Vertical) {
    return Headers.at(section);
  } else {
    return QVariant();
  }
}
