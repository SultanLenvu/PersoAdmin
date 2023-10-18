#include "hash_model.h"

HashModel::HashModel(QObject* parent) : QAbstractTableModel(parent) {
  setObjectName("HashModel");
  createMatchTables();
}

HashModel::~HashModel() {}

void HashModel::buildTransponderData(const QHash<QString, QString>* map) {
  // Проверка на существование
  if (!map) {
    return;
  }

  beginResetModel();

  // Очищаем старые данные
  HashTable.clear();

  // Устанавливаем новые данные
  for (QHash<QString, QString>::const_iterator it1 = map->constBegin();
       it1 != map->constEnd(); it1++) {
    HashTable.insert(TransponderDataMatchTable.value(it1.key()), it1.value());
  }

  endResetModel();
}

void HashModel::clear() {
  beginResetModel();

  HashTable.clear();

  endResetModel();
}

bool HashModel::isEmpty() const {
  return HashTable.isEmpty();
}

const QHash<QString, QVariant>* HashModel::map() const {
  return &HashTable;
}

int HashModel::columnCount(const QModelIndex& parent) const {
  if (HashTable.isEmpty()) {
    return 0;
  }

  return 1;
}

int HashModel::rowCount(const QModelIndex& parent) const {
  return HashTable.size();
}

QVariant HashModel::data(const QModelIndex& index, int role) const {
  if (index.column() > 1) {
    return QVariant();
  }

  if (index.row() > (HashTable.size())) {
    return QVariant();
  }

  if (role == Qt::DisplayRole) {
    return (HashTable.constBegin() + index.row()).value();
  } else
    return QVariant();
}

QVariant HashModel::headerData(int section,
                               Qt::Orientation orientation,
                               int role) const {
  if (section > (HashTable.size())) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    return "Значение";
  }

  if (orientation == Qt::Vertical) {
    return (HashTable.constBegin() + section).key();
  } else {
    return QVariant();
  }
}

void HashModel::createMatchTables() {
  TransponderDataMatchTable.insert("sn", "Серийный номер");
  TransponderDataMatchTable.insert("pan", "PAN");
  TransponderDataMatchTable.insert("issuer_name", "Компания-заказчик");
  TransponderDataMatchTable.insert("box_id", "Идентификатор бокса");
  TransponderDataMatchTable.insert("pallet_id", "Идентификатор паллеты");
  TransponderDataMatchTable.insert("order_id", "Идентификатор заказа");
  TransponderDataMatchTable.insert("transponder_model", "Модель транспондера");
}
