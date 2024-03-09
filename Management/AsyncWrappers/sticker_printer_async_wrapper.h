#ifndef STICKERPRINTERMANAGER_H
#define STICKERPRINTERMANAGER_H

#include "abstract_sticker_printer.h"
#include "loggable_object.h"
#include "named_object.h"
#include "progressable_async_wrapper.h"

class StickerPrinterAsyncWrapper final : public NamedObject,
                                         public ProgressableAsyncWrapper,
                                         public LoggableObject {
  Q_OBJECT
 private:
  std::unique_ptr<AbstractStickerPrinter> StickerPrinter;

 public:
  Q_INVOKABLE explicit StickerPrinterAsyncWrapper(const QString& name);
  ~StickerPrinterAsyncWrapper();

  // Own
 public slots:
  void printTransponderSticker(const std::shared_ptr<StringDictionary> param);
  void printBoxSticker(const std::shared_ptr<StringDictionary> param);
  void printPalletSticker(const std::shared_ptr<StringDictionary> param);
  void execCommandScript(const std::shared_ptr<QStringList> script);

 private:
  Q_DISABLE_COPY_MOVE(StickerPrinterAsyncWrapper)
};

#endif // STICKERPRINTERMANAGER_H
