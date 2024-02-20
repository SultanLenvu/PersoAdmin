#ifndef PERSOSERVERGUISUBKERNEL_H
#define PERSOSERVERGUISUBKERNEL_H

#include <QPlainTextEdit>

#include "abstract_gui_subkernel.h"
#include "hash_table_model.h"
#include "qplaintextedit.h"

class PersoServerGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 private:
  HashTableModel ProductionLine;
  HashTableModel Box;
  HashTableModel Transponder;

  QPlainTextEdit* FirmwareDisplay;

 public:
  explicit PersoServerGuiSubkernel(const QString& name);
  ~PersoServerGuiSubkernel();

 public:
  HashTableModel& productionLine(void);
  HashTableModel& box(void);
  HashTableModel& transponder(void);

 public:
  void setFirmwareDisplay(QPlainTextEdit* firmwareDisplay);

 public slots:  // Слоты для сигналов от GUI
  void connect(void);
  void disconnect(void);

  void echo(void);
  void logOn(void);
  void logOut(void);

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

 public slots:  // Слоты для сигналов от менеджеров
  void displayProductionLineData(const std::shared_ptr<StringDictionary> data);
  void displayBoxData(const std::shared_ptr<StringDictionary> data);
  void displayTransponderData(const std::shared_ptr<StringDictionary> data);
  void displayFirmware(const std::shared_ptr<QByteArray> firmware);

 private:
  void connectDependecies(void);
  void createModels(void);

 signals:
  void connect_signal(void);
  void disconnect_signal(void);

  void echo_signal(void);
  void logOn_signal(const std::shared_ptr<StringDictionary> param);
  void logOut_signal(void);

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

#endif  // PERSOSERVERGUISUBKERNEL_H
