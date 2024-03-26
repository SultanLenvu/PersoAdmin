#ifndef HASH_TABLE_MODEL_H
#define HASH_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QString>
#include <QVector>

#include "types.h"

class HashTableModel : public QAbstractTableModel {
 private:
  StringDictionary MatchTable;

  QHash<int, QString> Converter;
  StringDictionary Data;

 public:
  HashTableModel() = default;
  ~HashTableModel() = default;

  void setData(const StringDictionary& match);
  void setMatchTable(const StringDictionary& match);
  void clear(void);
  bool isEmpty(void) const;

  // QAbstractTableModel interface
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
};

#endif  // HASH_TABLE_MODEL_H
