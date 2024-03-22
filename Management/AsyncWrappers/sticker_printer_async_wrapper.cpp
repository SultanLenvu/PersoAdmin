#include "sticker_printer_async_wrapper.h"
#include "te310_printer.h"

StickerPrinterAsyncWrapper::StickerPrinterAsyncWrapper(const QString& name)
    : ProgressableAsyncWrapper(name) {
  StickerPrinter =
      std::unique_ptr<IStickerPrinter>(new TE310Printer("TE310Printer"));
}

StickerPrinterAsyncWrapper::~StickerPrinterAsyncWrapper() {}

void StickerPrinterAsyncWrapper::printTransponderSticker(
    const StringDictionary& param) {
  initOperation("printTransponderSticker");

  ReturnStatus ret = StickerPrinter->printTransponderSticker(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printTransponderSticker");
}

void StickerPrinterAsyncWrapper::printBoxSticker(
    const StringDictionary& param) {
  initOperation("printBoxSticker");

  ReturnStatus ret = StickerPrinter->printBoxSticker(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printBoxSticker");
}

void StickerPrinterAsyncWrapper::printPalletSticker(
    const StringDictionary& param) {
  initOperation("printPalletSticker");

  ReturnStatus ret = StickerPrinter->printPalletSticker(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printPalletSticker", ret);
    return;
  }

  completeOperation("printPalletSticker");
}

void StickerPrinterAsyncWrapper::execCommandScript(const QStringList& script) {
  initOperation("execCommandScript");

  ReturnStatus ret = StickerPrinter->exec(script);
  if (ret != ReturnStatus::NoError) {
    processOperationError("execPrinterStickerCommandScript", ret);
    return;
  }

  completeOperation("execCommandScript");
}
