#ifndef PERSOSERVERWIDGET_H
#define PERSOSERVERWIDGET_H

#include "abstract_user_interface.h"

class ServerUserInterface final : public AbstractUserInterface {
  Q_OBJECT

 private:
  QHBoxLayout* MainLayout;
  QVBoxLayout* SubLayout;

 private:  // Одиночные команды
  QGroupBox* RawCommandGroup;
  QVBoxLayout* RawCommandLayout;
  QComboBox* CommandComboBox;
  QPushButton* ExecuteCommandButton;

 private:  // Команды сборочного юнита
  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QGroupBox* InitGroup;
  QVBoxLayout* InitGroupLayout;
  QPushButton* LogOnPushButton;
  QPushButton* LogOutPushButton;

  QGroupBox* ProductionLineGroup;
  QVBoxLayout* ProductionLineGroupLayout;
  QPushButton* LaunchProductionLinePushButton;
  QPushButton* ShutdownProductionLinePushButton;

  QGroupBox* BoxGroup;
  QVBoxLayout* BoxGroupLayout;
  QPushButton* RequestBoxPushButton;
  QPushButton* RefundBoxPushButton;
  QPushButton* CompleteCurrentBoxPushButton;

  QGroupBox* TransponderGroup;
  QVBoxLayout* TransponderGroupLayout;
  QPushButton* ReleaseTransponderPushButton;
  QPushButton* RereleaseTransponderPushButton;
  QPushButton* RollbackTransponderPushButton;

  QSpacerItem* ControlPanelVS;

 private:  // Отображение данных
  QGroupBox* DataDisplayPanel;
  QGridLayout* DataDisplayLayout;

  QGroupBox* ProductionLineDataGroup;
  QVBoxLayout* ProductionLineDataLayout;
  QTableView* ProductionLineDataView;

  QGroupBox* BoxDataGroup;
  QVBoxLayout* BoxDataLayout;
  QTableView* BoxDataView;

  QGroupBox* TransponderDataGroup;
  QVBoxLayout* TransponderDataLayout;
  QTableView* TransponderDataView;

  QGroupBox* FirmwareGroup;
  QVBoxLayout* FirmwareLayout;
  QListView* FirmwareView;

 public:
  explicit ServerUserInterface(QWidget* parent = nullptr);
  ~ServerUserInterface() = default;

 private:
  void createWidgets(void);
  void createControlPanel(void);
  void createRawCommandGroup(void);
  void createInitGroup(void);
  void createProductionLineGroup(void);
  void createBoxGroup(void);
  void createTransponderGroup(void);
  void createPrintingGroup(void);
  void createDataDisplayPanel(void);

  void connectDependencies(void);

 private slots:
  void onExecuteCommandButton_slot(void);

 signals:
  void executeCommand_signal(const QString& name);
};

#endif  // PERSOSERVERWIDGET_H
