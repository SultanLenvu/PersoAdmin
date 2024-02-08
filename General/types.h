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
  SyntaxError,
  ConsistencyViolation,
  FileOpenError,
  DynamicLibraryMissing,

  DatabaseConnectionError,
  DatabaseTransactionError,
  DatabaseQueryError,

  RegisterFileError,

  ProductionLineMissed,
  ProductionLineLinkError,
  ProductionLineRollbackLimit,

  OrderMissed,
  OrderRemovingError,
  OtherOrderInProcess,
  OrderNotInProcess,
  IssuerMissed,
  MasterKeysMissed,

  ServerConnectionError,
  ServerConnectionMissed,
  ServerNotResponding,
  ServerDataTransmittingError,
  ServerResponseSyntaxError,
  ServerResponseDataBlockError,
  ServerResponseProcessingError,

  ClientCommandParamError,
  ServerInternalError,

  ProductionContextNotValid,

  OrderMultiplyAssembly,
  OrderInProcessMissed,
  OrderCompletelyAssembled,

  BoxMissed,
  BoxAlreadyRequested,
  FreeBoxMissed,
  BoxIsEmty,
  BoxOverflow,
  BoxCompletelyAssembled,
  BoxNotCompletelyAssembled,

  PalletMissed,
  PalletIsEmpty,
  PalletOverflow,

  TransponderMissed,
  TransponderIncorrectRerelease,
  TransponderIdenticalUcidError,
  TransponderRollbackLimit,

  ProductionLineContextNotAuthorized,
  ProductionLineNotLaunched,
  ProductionLineLaunchError,
  ProductionLineAlreadyLaunched,
  ProductionLineAlreadyInProcess,
  ProductionLineNotActive,
  ProductionLineCompleted,
  ProductionLineNotInProcess,

  StickerPrintingFailed,
  StickerPrinterDriverMissed,
  StickerPrinterLibraryMissing,
  StickerPrinterInitError,
  StickerPrinterConnectionError,
  StickerPrintError,
  LastStickerMissed,

  UnknownError,
};

#endif  // TYPES_H
