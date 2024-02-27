#ifndef STICKERPRINTERMANAGER_H
#define STICKERPRINTERMANAGER_H

#include "abstract_async_wrapper.h"
#include "abstract_sticker_printer.h"

class StickerPrinterAsyncWrapper final : public AbstractAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<AbstractStickerPrinter> StickerPrinter;

 public:
  explicit StickerPrinterAsyncWrapper(const QString& name);
  ~StickerPrinterAsyncWrapper();

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;

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