#include "sticker_printer_gui_subkernel.h"
#include "global_environment.h"
#include "sticker_printer_async_wrapper.h"
#include "string_input_dialog.h"

StickerPrinterGuiSubkernel::StickerPrinterGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}

StickerPrinterGuiSubkernel::~StickerPrinterGuiSubkernel() {}

void StickerPrinterGuiSubkernel::printTransponderSticker() {
  StringDictionary& param(new StringDictionary);

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printTransponderSticker_signal(param);
}

void StickerPrinterGuiSubkernel::printBoxSticker() {
  StringDictionary& param(new StringDictionary);

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printBoxSticker_signal(param);
}

void StickerPrinterGuiSubkernel::printPalletSticker() {
  StringDictionary& param(new StringDictionary);

  StringInputDialog dialog("id");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printPalletSticker_signal(param);
}

void StickerPrinterGuiSubkernel::execCommandScript(
    const std::shared_ptr<QStringList> script) {
  emit clearLogDisplay();
  emit execCommandScript_signal(script);
}

void StickerPrinterGuiSubkernel::connectDependecies() {
  const StickerPrinterAsyncWrapper* spm =
      GlobalEnvironment::instance()
          ->getObject<const StickerPrinterAsyncWrapper>(
              "StickerPrinterAsyncWrapper");

  connect(this, &StickerPrinterGuiSubkernel::printTransponderSticker_signal,
          spm, &StickerPrinterAsyncWrapper::printTransponderSticker);
  connect(this, &StickerPrinterGuiSubkernel::printBoxSticker_signal, spm,
          &StickerPrinterAsyncWrapper::printBoxSticker);
  connect(this, &StickerPrinterGuiSubkernel::printPalletSticker_signal, spm,
          &StickerPrinterAsyncWrapper::printPalletSticker);
  connect(this, &StickerPrinterGuiSubkernel::execCommandScript_signal, spm,
          &StickerPrinterAsyncWrapper::execCommandScript);
}
