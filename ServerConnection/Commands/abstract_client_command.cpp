#include "abstract_client_command.h"

#include <QDataStream>
#include <QIODevice>
#include <QJsonDocument>

AbstractClientCommand::AbstractClientCommand(const QString& name)
    : NamedObject(name) {
  createCrtMap();
  createCrtLogMap();
}

AbstractClientCommand::~AbstractClientCommand() {}

void AbstractClientCommand::clear() {
  Request = QJsonObject();
  Response = QJsonObject();
}

void AbstractClientCommand::generateDataBlock(QByteArray& dataBlock) {
  QJsonDocument requestDocument(Request);

  // Инициализируем блок данных и сериализатор
  dataBlock.clear();
  QDataStream serializator(&dataBlock, QIODevice::WriteOnly);
  serializator.setVersion(QDataStream::Qt_6_4);

  // Формируем единый блок данных для отправки
  serializator << uint32_t(0) << requestDocument.toJson();
  serializator.device()->seek(0);
  serializator << uint32_t(dataBlock.size() - sizeof(uint32_t));

  sendLog(QString("Размер отправляемого блока данных: %1")
              .arg(QString::number(dataBlock.size())));
  // Ограничиваем вывод в лог полученного блока данных, чтобы не было фризов при
  // отрисовке
  sendLog(QString("Отправляемый блок данных: %1").arg(dataBlock));
}

bool AbstractClientCommand::processDataBlock(const QByteArray& dataBlock) {
  QJsonParseError status;
  QJsonDocument responseDocument = QJsonDocument::fromJson(dataBlock, &status);

  // Если пришел некорректный JSON
  if (status.error != QJsonParseError::NoError) {
    sendLog("Получена ошибка при обработке полученного блока данных.");
    return false;
  }

  // Выделяем список пар ключ-значение из JSON-файла
  Response = responseDocument.object();
  return true;
}

ReturnStatus AbstractClientCommand::processReturnStatus(const QString& ret) {
  CommandReturnStatus cret = static_cast<CommandReturnStatus>(ret.toInt());

  sendLog(CrtLogMap[cret]);

  return CrtMap[cret];
}

void AbstractClientCommand::createCrtMap() {
  CrtMap[NoError] = ReturnStatus::NoError;
  CrtMap[DynamicLibraryMissing] = ReturnStatus::ServerInternalError;
  CrtMap[ParameterError] = ReturnStatus::ServerInternalError;
  CrtMap[SyntaxError] = ReturnStatus::ServerInternalError;
  CrtMap[ConsistencyViolation] = ReturnStatus::ServerInternalError;
  CrtMap[FileOpenError] = ReturnStatus::ServerInternalError;

  CrtMap[DatabaseConnectionError] = ReturnStatus::ServerInternalError;
  CrtMap[DatabaseTransactionError] = ReturnStatus::ServerInternalError;
  CrtMap[DatabaseQueryError] = ReturnStatus::ServerInternalError;

  CrtMap[FirmwareGeneratorInitError] = ReturnStatus::ServerInternalError;
  CrtMap[FirmwareGenerationError] = ReturnStatus::ServerInternalError;

  CrtMap[ProductionContextNotValid] = ReturnStatus::ProductionContextNotValid;

  CrtMap[RecordMissed] = ReturnStatus::ServerInternalError;
  CrtMap[ProductionLineMissed] = ReturnStatus::ProductionLineMissed;
  CrtMap[TranspoderMissed] = ReturnStatus::TransponderMissed;
  CrtMap[BoxMissed] = ReturnStatus::BoxMissed;
  CrtMap[PalletMissed] = ReturnStatus::PalletMissed;
  CrtMap[OrderMissed] = ReturnStatus::OrderMissed;
  CrtMap[IssuerMissed] = ReturnStatus::IssuerMissed;
  CrtMap[MasterKeysMissed] = ReturnStatus::MasterKeysMissed;

  CrtMap[OrderMultiplyAssembly] = ReturnStatus::OrderMultiplyAssembly;
  CrtMap[OrderInProcessMissed] = ReturnStatus::OrderInProcessMissed;
  CrtMap[OrderCompletelyAssembled] = ReturnStatus::OrderCompletelyAssembled;

  CrtMap[BoxAlreadyRequested] = ReturnStatus::BoxAlreadyRequested;
  CrtMap[FreeBoxMissed] = ReturnStatus::FreeBoxMissed;
  CrtMap[BoxIsEmty] = ReturnStatus::BoxIsEmty;
  CrtMap[BoxOverflow] = ReturnStatus::BoxOverflow;
  CrtMap[BoxCompletelyAssembled] = ReturnStatus::BoxCompletelyAssembled;
  CrtMap[BoxNotCompletelyAssembled] = ReturnStatus::BoxNotCompletelyAssembled;

  CrtMap[PalletIsEmpty] = ReturnStatus::PalletIsEmpty;
  CrtMap[PalletOverflow] = ReturnStatus::PalletOverflow;

  CrtMap[TransponderRepeatRelease] = ReturnStatus::ServerInternalError;
  CrtMap[TransponderNotReleasedEarlier] = ReturnStatus::ServerInternalError;
  CrtMap[TransponderNotAwaitingConfirmation] =
      ReturnStatus::ServerInternalError;
  CrtMap[TransponderIncorrectRerelease] =
      ReturnStatus::TransponderIncorrectRerelease;
  CrtMap[TransponderIdenticalUcidError] =
      ReturnStatus::TransponderIdenticalUcidError;
  CrtMap[TransponderRollbackLimit] = ReturnStatus::TransponderRollbackLimit;

  CrtMap[ProductionLineLaunchSystemInitError] =
      ReturnStatus::ServerInternalError;
  CrtMap[ProductionLineContextNotAuthorized] =
      ReturnStatus::ProductionLineContextNotAuthorized;
  CrtMap[ProductionLineNotActive] = ReturnStatus::ProductionLineNotActive;
  CrtMap[ProductionLineNotLaunched] = ReturnStatus::ProductionLineNotLaunched;
  CrtMap[ProductionLineNotInProcess] = ReturnStatus::ProductionLineNotInProcess;
  CrtMap[ProductionLineLaunchError] = ReturnStatus::ProductionLineLaunchError;
  CrtMap[ProductionLineAlreadyLaunched] =
      ReturnStatus::ProductionLineAlreadyLaunched;
  CrtMap[ProductionLineAlreadyInProcess] =
      ReturnStatus::ProductionLineAlreadyInProcess;
  CrtMap[ProductionLineCompleted] = ReturnStatus::ProductionLineCompleted;
  CrtMap[ProductionLineShutdownError] = ReturnStatus::ServerInternalError;

  CrtMap[StickerPrinterDriverMissed] = ReturnStatus::ServerInternalError;
  CrtMap[StickerPrinterLibraryMissing] = ReturnStatus::ServerInternalError;
  CrtMap[StickerPrinterConnectionError] = ReturnStatus::ServerInternalError;
  CrtMap[StickerPrintError] = ReturnStatus::ServerInternalError;
  CrtMap[LastStickerMissed] = ReturnStatus::ServerInternalError;

  CrtMap[Unknown] = ReturnStatus::ServerInternalError;
}

void AbstractClientCommand::createCrtLogMap() {
  CrtLogMap[NoError] = "Server return status: NoError. ";
  CrtLogMap[DynamicLibraryMissing] =
      "Server return status: DynamicLibraryMissing. ";
  CrtLogMap[ParameterError] = "Server return status: ParameterError";
  CrtLogMap[SyntaxError] = "Server return status: SyntaxError";
  CrtLogMap[ConsistencyViolation] =
      "Server return status: ConsistencyViolation";
  CrtLogMap[FileOpenError] = "Server return status: FileOpenError";

  CrtLogMap[DatabaseConnectionError] =
      "Server return status: DatabaseConnectionError";
  CrtLogMap[DatabaseTransactionError] =
      "Server return status: DatabaseTransactionError";
  CrtLogMap[DatabaseQueryError] = "Server return status: DatabaseQueryError";

  CrtLogMap[FirmwareGeneratorInitError] =
      "Server return status: FirmwareGeneratorInitError";
  CrtLogMap[FirmwareGenerationError] =
      "Server return status: FirmwareGenerationError";

  CrtLogMap[ProductionContextNotValid] =
      "Server return status: ProductionContextNotValid";

  CrtLogMap[RecordMissed] = "Server return status: RecordMissed";
  CrtLogMap[ProductionLineMissed] =
      "Server return status: ProductionLineMissed";
  CrtLogMap[TranspoderMissed] = "Server return status: TranspoderMissed";
  CrtLogMap[BoxMissed] = "Server return status: BoxMissed";
  CrtLogMap[PalletMissed] = "Server return status: PalletMissed";
  CrtLogMap[OrderMissed] = "Server return status: OrderMissed";
  CrtLogMap[IssuerMissed] = "Server return status: IssuerMissed";
  CrtLogMap[MasterKeysMissed] = "Server return status: MasterKeysMissed";

  CrtLogMap[OrderMultiplyAssembly] =
      "Server return status: OrderMultiplyAssembly";
  CrtLogMap[OrderInProcessMissed] =
      "Server return status: OrderAssemblyMissingы";
  CrtLogMap[OrderCompletelyAssembled] =
      "Server return status: OrderCompletelyAssembled";

  CrtLogMap[BoxAlreadyRequested] = "Server return status: BoxAlreadyRequested";
  CrtLogMap[FreeBoxMissed] = "Server return status: FreeBoxMissed";
  CrtLogMap[BoxIsEmty] = "Server return status: BoxIsEmty";
  CrtLogMap[BoxOverflow] = "Server return status: BoxOverflow";
  CrtLogMap[BoxCompletelyAssembled] =
      "Server return status: BoxCompletelyAssembled";
  CrtLogMap[BoxNotCompletelyAssembled] =
      "Server return status: BoxNotCompletelyAssembled";

  CrtLogMap[PalletIsEmpty] = "Server return status: PalletIsEmpty";
  CrtLogMap[PalletOverflow] = "Server return status: PalletOverflow";

  CrtLogMap[TransponderRepeatRelease] =
      "Server return status: TransponderRepeatReleases";
  CrtLogMap[TransponderNotReleasedEarlier] =
      "Server return status: TransponderNotReleasedEarlier";
  CrtLogMap[TransponderNotAwaitingConfirmation] =
      "Server return status: TransponderNotAwaitingConfirmation";
  CrtLogMap[TransponderIncorrectRerelease] =
      "Server return status: TransponderIncorrectRerelease";
  CrtLogMap[TransponderIdenticalUcidError] =
      "Server return status: TransponderIdenticalUcidError";
  CrtLogMap[TransponderRollbackLimit] =
      "Server return status: TransponderRollbackLimit";

  CrtLogMap[ProductionLineLaunchSystemInitError] =
      "Server return status: ProductionLineLaunchSystemInitError";
  CrtLogMap[ProductionLineContextNotAuthorized] =
      "Server return status: ProductionLineContextNotAuthorized";
  CrtLogMap[ProductionLineNotActive] =
      "Server return status: ProductionLineNotActive";
  CrtLogMap[ProductionLineNotLaunched] =
      "Server return status: ProductionLineNotLaunched";
  CrtLogMap[ProductionLineNotInProcess] =
      "Server return status: ProductionLineNotInProcess";
  CrtLogMap[ProductionLineLaunchError] =
      "Server return status: ProductionLineLaunchError";
  CrtLogMap[ProductionLineAlreadyLaunched] =
      "Server return status: ProductionLineAlreadyLaunched";
  CrtLogMap[ProductionLineAlreadyInProcess] =
      "Server return status: ProductionLineAlreadyInProcess";
  CrtLogMap[ProductionLineCompleted] =
      "Server return status: ProductionLineCompleted";
  CrtLogMap[ProductionLineShutdownError] =
      "Server return status: ProductionLineShutdownError";

  CrtLogMap[StickerPrinterDriverMissed] =
      "Server return status: StickerPrinterDriverMissed";
  CrtLogMap[StickerPrinterLibraryMissing] =
      "Server return status: StickerPrinterLibraryMissing";
  CrtLogMap[StickerPrinterConnectionError] =
      "Server return status: StickerPrinterConnectionError";
  CrtLogMap[StickerPrintError] = "Server return status: StickerPrintError";
  CrtLogMap[LastStickerMissed] = "Server return status: LastStickerMissed";

  CrtLogMap[Unknown] = "Server return status: Unknown";
}
