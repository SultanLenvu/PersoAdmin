#ifndef ASSEMBLYUNITGUISUBKERNEL_H
#define ASSEMBLYUNITGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"
#include "hash_table_model.h"

class AssemblyUnitGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 private:
  HashTableModel ProductionLine;
  HashTableModel Box;
  HashTableModel Transponder;

 public:
  explicit AssemblyUnitGuiSubkernel(const QString& name);
  ~AssemblyUnitGuiSubkernel() = default;

 public:
  HashTableModel& productionLine(void);
  HashTableModel& box(void);
  HashTableModel& transponder(void);

 public slots:  // Слоты для сигналов от GUI
  void logOn(void);
  void logOut(void);

  void requestBox(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(void);
  void rollbackTransponder(void);

 public slots:  // Слоты для сигналов от менеджеров
  void displayProductionLineData(const std::shared_ptr<StringDictionary> data);
  void displayBoxData(const std::shared_ptr<StringDictionary> data);
  void displayTransponderData(const std::shared_ptr<StringDictionary> data);
  void displayFirmware(const std::shared_ptr<QByteArray> firmware);

 private:
  void connectDependecies(void);
  void createModels(void);

 signals:
  void logOn_signal(const std::shared_ptr<StringDictionary> param);
  void logOut_signal(void);

  void requestBox_signal(void);
  void refundCurrentBox_signal(void);
  void completeCurrentBox_signal(void);

  void releaseTransponder_signal(void);
  void rereleaseTransponder_signal(
      const std::shared_ptr<StringDictionary> param);
  void rollbackTransponder_signal(void);
};

#endif // ASSEMBLYUNITGUISUBKERNEL_H
