#ifndef SQL_RESPONSE_MODEL_H
#define SQL_RESPONSE_MODEL_H

#include <QAbstractTableModel>

#include "sql_query_values.h"

class SqlResponseModel final : public QAbstractTableModel {
 private:
  std::shared_ptr<SqlQueryValues> Response;

 public:
  SqlResponseModel();
  ~SqlResponseModel();

  void setResponse(std::shared_ptr<SqlQueryValues> response);
  void clear(void);
  bool isEmpty(void) const;

 public:  // QAbstractTableModel interface
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
};

#endif  // SQL_RESPONSE_MODEL_H
