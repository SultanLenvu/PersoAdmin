#ifndef HASH_TABLE_MODEL_H
#define HASH_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include <QString>
#include <QVector>

#include "types.h"

class HashTableModel : public QAbstractTableModel {
 private:
  std::shared_ptr<StringDictionary> MatchTable;

  QVector<QVariant> Values;
  QVector<QVariant> Headers;

 public:
  HashTableModel();
  ~HashTableModel();

  void setData(const StringDictionary& table);
  void setMatchTable(std::shared_ptr<StringDictionary> match);
  void clear(void);
  bool isEmpty(void) const;

  // QAbstractTableModel interface
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

 private:
  Q_DISABLE_COPY_MOVE(HashTableModel);
};

#endif  // HASH_TABLE_MODEL_H
