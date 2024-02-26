#include "sticker_printer_async_wrapper.h"
#include "te310_printer.h"

StickerPrinterManager::StickerPrinterManager(const QString& name)
    : AbstractManager(name) {
}

StickerPrinterManager::~StickerPrinterManager() {}

void StickerPrinterManager::onInstanceThreadStarted() {
  StickerPrinter = std::unique_ptr<AbstractStickerPrinter>(
      new TE310Printer("StickerPrinter"));
}

void StickerPrinterManager::printTransponderSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printTransponderSticker");

  ReturnStatus ret = StickerPrinter->printTransponderSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printTransponderSticker");
}

void StickerPrinterManager::printBoxSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printBoxSticker");

  ReturnStatus ret = StickerPrinter->printBoxSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printBoxSticker");
}

void StickerPrinterManager::printPalletSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printPalletSticker");

  ReturnStatus ret = StickerPrinter->printPalletSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printPalletSticker", ret);
    return;
  }

  completeOperation("printPalletSticker");
}

void StickerPrinterManager::execCommandScript(
    const std::shared_ptr<QStringList> script) {
  initOperation("execCommandScript");

  ReturnStatus ret = StickerPrinter->exec(*script);
  if (ret != ReturnStatus::NoError) {
    processOperationError("execPrinterStickerCommandScript", ret);
    return;
  }

  completeOperation("execCommandScript");
}
