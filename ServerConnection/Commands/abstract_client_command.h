#ifndef ABSTRACTCLIENTCOMMAND_H
#define ABSTRACTCLIENTCOMMAND_H

#include <QJsonObject>

#include "loggable_object.h"
#include "named_object.h"
#include "types.h"

class AbstractClientCommand : public NamedObject, public LoggableObject {
 public:
  explicit AbstractClientCommand(const QString& name);
  virtual ~AbstractClientCommand();

 public:
  virtual const QString& name(void) = 0;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) = 0;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary responseData) = 0;

 public:
  void clear(void);

 protected:
  void generateDataBlock(QByteArray& dataBlock);
  bool processDataBlock(const QByteArray& dataBlock);
  ReturnStatus processReturnStatus(const QString& ret);

 private:
  Q_DISABLE_COPY_MOVE(AbstractClientCommand);

  void createCrtMap(void);
  void createCrtLogMap(void);

 private:
  enum CommandReturnStatus {
    NoError = 0,
    DynamicLibraryMissing,
    ParameterError,
    SyntaxError,
    ConsistencyViolation,
    FileOpenError,

    DatabaseConnectionError,
    DatabaseTransactionError,
    DatabaseQueryError,

    FirmwareGeneratorInitError,
    FirmwareGenerationError,

    ProductionContextNotValid,

    RecordMissed,
    ProductionLineMissed,
    TranspoderMissed,
    BoxMissed,
    PalletMissed,
    OrderMissed,
    IssuerMissed,
    MasterKeysMissed,

    OrderMultiplyAssembly,
    OrderInProcessMissed,
    OrderCompletelyAssembled,

    BoxAlreadyRequested,
    FreeBoxMissed,
    BoxIsEmty,
    BoxOverflow,
    BoxCompletelyAssembled,
    BoxNotCompletelyAssembled,

    PalletIsEmpty,
    PalletOverflow,

    TransponderRepeatRelease,
    TransponderNotReleasedEarlier,
    TransponderNotAwaitingConfirmation,
    TransponderIncorrectRerelease,
    TransponderIdenticalUcidError,
    TransponderRollbackLimit,

    ProductionLineLaunchSystemInitError,
    ProductionLineContextNotAuthorized,
    ProductionLineNotActive,
    ProductionLineNotLaunched,
    ProductionLineNotInProcess,
    ProductionLineLaunchError,
    ProductionLineAlreadyLaunched,
    ProductionLineAlreadyInProcess,
    ProductionLineCompleted,
    ProductionLineShutdownError,

    StickerPrinterDriverMissed,
    StickerPrinterLibraryMissing,
    StickerPrinterConnectionError,
    StickerPrintError,
    LastStickerMissed,

    Unknown,
  };

 protected:
  QJsonObject Request;
  QJsonObject Response;

  std::unordered_map<CommandReturnStatus, ReturnStatus> CrtMap;
  std::unordered_map<CommandReturnStatus, QString> CrtLogMap;
};

#endif  // ABSTRACTCLIENTCOMMAND_H
