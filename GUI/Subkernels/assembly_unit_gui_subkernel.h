#ifndef ASSEMBLYUNITGUISUBKERNEL_H
#define ASSEMBLYUNITGUISUBKERNEL_H

#include <QStringListModel>

#include "abstract_gui_subkernel.h"
#include "hash_table_model.h"
#include "types.h"

class AssemblyUnitGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT
  friend class ServerUserInterface;

 private:
  HashTableModel ProductionLineModel;
  HashTableModel BoxModel;
  HashTableModel TransponderModel;

  QStringListModel FirmwareModel;

 public:
  explicit AssemblyUnitGuiSubkernel(const QString& name);
  ~AssemblyUnitGuiSubkernel() = default;

 private slots:  // Слоты для сигналов от GUI
  void logOn(void);
  void logOut(void);

  void requestBox(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(void);
  void rollbackTransponder(void);

 public slots:
  void displayProductionLineData(const StringDictionary& data);
  void displayBoxData(const StringDictionary& data);
  void displayTransponderData(const StringDictionary& data);
  void displayFirmware(const QStringList& firmware);

 private:
  Q_DISABLE_COPY_MOVE(AssemblyUnitGuiSubkernel)
  void createModels(void);
  void connectDependecies(void);

 signals:
  void logOn_signal(const StringDictionary& param);
  void logOut_signal(void);

  void requestBox_signal(void);
  void refundCurrentBox_signal(void);
  void completeCurrentBox_signal(void);

  void releaseTransponder_signal(void);
  void rereleaseTransponder_signal(
      const StringDictionary& param);
  void rollbackTransponder_signal(void);
};

#endif // ASSEMBLYUNITGUISUBKERNEL_H
