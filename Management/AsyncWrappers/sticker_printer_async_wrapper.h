#ifndef STICKERPRINTERMANAGER_H
#define STICKERPRINTERMANAGER_H

#include "i_sticker_printer.h"
#include "progressable_async_wrapper.h"

class StickerPrinterAsyncWrapper final : public ProgressableAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<IStickerPrinter> StickerPrinter;

 public:
  Q_INVOKABLE explicit StickerPrinterAsyncWrapper(const QString& name);
  ~StickerPrinterAsyncWrapper();

  // Own
 public slots:
  void printTransponderSticker(const StringDictionary& param);
  void printBoxSticker(const StringDictionary& param);
  void printPalletSticker(const StringDictionary& param);
  void execCommandScript(const QStringList&& script);

 private:
  Q_DISABLE_COPY_MOVE(StickerPrinterAsyncWrapper)
};

#endif // STICKERPRINTERMANAGER_H
