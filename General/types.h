#ifndef TYPES_H
#define TYPES_H

#include <QHash>

using StringDictionary = QHash<QString, QString>;

template <typename T>
using SharedVector = std::shared_ptr<QVector<T>>;

enum class ProductionLineState {
  NotActive,
  Idle,
  Launched,
  NoError,
};

enum class ReturnStatus {
  NoError,
  ParameterError,

  DatabaseConnectionError,
  DatabaseTransactionError,
  DatabaseQueryError,

  RegisterFileError,
  TransponderMissed,

  BoxMissed,
  PalletMissed,
  OrderMissed,

  OrderAlreadyReleased,
  OrderNotReleased,

  ProductionLineMissed,
  ProductionLineLinkError,
  ProductionLineRollbackLimit,

  OrderRemovingError,
  OtherOrderInProcess,
  OrderNotInProcess,

  MultipleActiveOrders,
  FreeBoxMissed,
  UnknownError,
};

#endif  // TYPES_H
