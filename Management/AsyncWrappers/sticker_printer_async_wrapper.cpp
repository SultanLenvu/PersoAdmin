#include "sticker_printer_async_wrapper.h"
#include "te310_printer.h"

StickerPrinterAsyncWrapper::StickerPrinterAsyncWrapper(const QString& name)
    : AbstractAsyncWrapper(name) {}

StickerPrinterAsyncWrapper::~StickerPrinterAsyncWrapper() {}

void StickerPrinterAsyncWrapper::onInstanceThreadStarted() {
  StickerPrinter = std::unique_ptr<AbstractStickerPrinter>(
      new TE310Printer("StickerPrinter"));
}

void StickerPrinterAsyncWrapper::printTransponderSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printTransponderSticker");

  ReturnStatus ret = StickerPrinter->printTransponderSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printTransponderSticker");
}

void StickerPrinterAsyncWrapper::printBoxSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printBoxSticker");

  ReturnStatus ret = StickerPrinter->printBoxSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printBoxSticker");
}

void StickerPrinterAsyncWrapper::printPalletSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printPalletSticker");

  ReturnStatus ret = StickerPrinter->printPalletSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printPalletSticker", ret);
    return;
  }

  completeOperation("printPalletSticker");
}

void StickerPrinterAsyncWrapper::execCommandScript(
    const std::shared_ptr<QStringList> script) {
  initOperation("execCommandScript");

  ReturnStatus ret = StickerPrinter->exec(*script);
  if (ret != ReturnStatus::NoError) {
    processOperationError("execPrinterStickerCommandScript", ret);
    return;
  }

  completeOperation("execCommandScript");
}