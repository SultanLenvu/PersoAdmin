#ifndef STICKERPRINTERGUISUBKERNEL_H
#define STICKERPRINTERGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"
#include "types.h"

class StickerPrinterGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT
 public:
  explicit StickerPrinterGuiSubkernel(const QString& name);
  ~StickerPrinterGuiSubkernel();

 public slots:  // Слоты для сигналов от GUI
  void printTransponderSticker(void);
  void printBoxSticker(void);
  void printPalletSticker(void);
  void execCommandScript(const QStringList&& script);

 public slots:  // Слоты для сигналов от менеджеров

 private:
  void connectDependecies(void);

 signals:
  void printTransponderSticker_signal(
      const StringDictionary& param);
  void printBoxSticker_signal(const StringDictionary& param);
  void printPalletSticker_signal(const StringDictionary& param);
  void execCommandScript_signal(const QStringList&& script);
};

#endif // STICKERPRINTERGUISUBKERNEL_H
