#include "sql_response_model.h"

SqlResponseModel::SqlResponseModel() : QAbstractTableModel(nullptr) {}

SqlResponseModel::~SqlResponseModel() {}

void SqlResponseModel::setResponse(std::shared_ptr<SqlQueryValues> response) {
  beginResetModel();

  Response = response;

  endResetModel();
}

void SqlResponseModel::clear() {
  beginResetModel();

  Response.reset();

  endResetModel();
}

bool SqlResponseModel::isEmpty() const {
  return Response == nullptr ? true : false;
}

int SqlResponseModel::columnCount(const QModelIndex& parent) const {
  if (!Response) {
    return 0;
  }

  return Response->fieldCount();
}

int SqlResponseModel::rowCount(const QModelIndex& parent) const {
  if (!Response) {
    return 0;
  }

  return Response->recordCount();
}

QVariant SqlResponseModel::data(const QModelIndex& index, int role) const {
  int32_t col = index.column();
  int32_t row = index.row();

  if (col >= Response->fieldCount())
    return QVariant();

  if (row >= Response->recordCount())
    return QVariant();

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  return Response->get(row, col);
}

QVariant SqlResponseModel::headerData(int section,
                                      Qt::Orientation orientation,
                                      int role) const {
  if (section >= Response->fieldCount()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation != Qt::Horizontal) {
    return QVariant();
  }

  return Response->fieldName(section);
}
