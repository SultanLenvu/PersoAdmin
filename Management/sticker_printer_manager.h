#ifndef STICKERPRINTERMANAGER_H
#define STICKERPRINTERMANAGER_H

#include "abstract_manager.h"
#include "abstract_sticker_printer.h"

class StickerPrinterManager : public AbstractManager
{
  Q_OBJECT
 private:
  std::unique_ptr<AbstractStickerPrinter> StickerPrinter;

 public:
  explicit StickerPrinterManager(const QString& name);
  ~StickerPrinterManager();

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;

  // Own
 public slots:
  void printTransponderSticker(const std::shared_ptr<StringDictionary> param);
  void printBoxSticker(const std::shared_ptr<StringDictionary> param);
  void printPalletSticker(const std::shared_ptr<StringDictionary> param);
  void execCommandScript(const std::shared_ptr<QStringList> script);

 private:
  void loadSettings(void);
};

#endif // STICKERPRINTERMANAGER_H
