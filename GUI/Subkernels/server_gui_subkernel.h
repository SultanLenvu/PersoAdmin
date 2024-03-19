#ifndef SERVERGUISUBKERNEL_H
#define SERVERGUISUBKERNEL_H

#include <QPlainTextEdit>

#include "abstract_gui_subkernel.h"
#include "types.h"

class ServerGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 public:
  explicit ServerGuiSubkernel(const QString& name);
  ~ServerGuiSubkernel() = default;

 public slots:  // Слоты для сигналов от GUI
  void executeCommand(const QString& name);

  void connect(void);
  void disconnect(void);

  void echo(void);
  void launchProductionLine(void);
  void shutdownProductionLine(void);
  void getProductionLineData(void);

  void requestBox(void);
  void getCurrentBoxData(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(void);
  void rollbackTransponder(void);
  void getCurrentTransponderData(void);
  void getTransponderData(void);

  void printBoxSticker(void);
  void printLastBoxSticker(void);
  void printPalletSticker(void);
  void printLastPalletSticker(void);

 private:
  void connectDependecies(void);

 signals:
  void connect_signal(void);
  void disconnect_signal(void);

  void echo_signal(void);

  void launchProductionLine_signal(
      const std::shared_ptr<StringDictionary> param);
  void shutdownProductionLine_signal(void);
  void getProductionLineData_signal(void);

  void requestBox_signal(void);
  void getCurrentBoxData_signal(void);
  void refundCurrentBox_signal(void);
  void completeCurrentBox_signal(void);

  void releaseTransponder_signal(void);
  void rereleaseTransponder_signal(
      const std::shared_ptr<StringDictionary> param);
  void rollbackTransponder_signal(void);
  void getCurrentTransponderData_signal(void);
  void getTransponderData_signal(const std::shared_ptr<StringDictionary> param);

  void printBoxSticker_signal(const std::shared_ptr<StringDictionary> data);
  void printLastBoxSticker_signal(void);
  void printPalletSticker_signal(const std::shared_ptr<StringDictionary> data);
  void printLastPalletSticker_signal(void);
};

#endif  // SERVERGUISUBKERNEL_H
