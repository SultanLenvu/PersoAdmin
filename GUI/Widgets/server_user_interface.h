#ifndef PERSOSERVERWIDGET_H
#define PERSOSERVERWIDGET_H

#include "abstract_user_interface.h"

class ServerUserInterface final : public AbstractUserInterface {
  Q_OBJECT

 private:
  QHBoxLayout* MainLayout;

  // Панель управления
  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QGroupBox* InitGroup;
  QVBoxLayout* InitGroupLayout;
  QPushButton* ConnectPushButton;
  QPushButton* DisconnectPushButton;
  QPushButton* EchoPushButton;
  QPushButton* LogOnPushButton;
  QPushButton* LogOutPushButton;

  QGroupBox* ProductionLineGroup;
  QVBoxLayout* ProductionLineGroupLayout;
  QPushButton* LaunchProductionLinePushButton;
  QPushButton* ShutdownProductionLinePushButton;
  QPushButton* GetProductionLineDataPushButton;

  QGroupBox* BoxGroup;
  QVBoxLayout* BoxGroupLayout;
  QPushButton* RequestBoxPushButton;
  QPushButton* GetCurrentBoxDataPushButton;
  QPushButton* RefundBoxPushButton;
  QPushButton* CompleteCurrentBoxPushButton;

  QGroupBox* TransponderGroup;
  QVBoxLayout* TransponderGroupLayout;
  QPushButton* ReleaseTransponderPushButton;
  QPushButton* RereleaseTransponderPushButton;
  QPushButton* RollbackTransponderPushButton;
  QPushButton* GetCurrentTransponderDataPushButton;
  QPushButton* GetTransponderDataPushButton;

  QGroupBox* PrintingGroup;
  QVBoxLayout* PrintingGroupLayout;
  QPushButton* PrintBoxStickerOnServerPushButton;
  QPushButton* PrintLastBoxStickerOnServerPushButton;
  QPushButton* PrintPalletStickerOnServerPushButton;
  QPushButton* PrintLastPalletStickerOnServerPushButton;

  QSpacerItem* ControlPanelVS;

  // Отображение данных
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

  QGroupBox* AssembledFirmwareGroup;
  QVBoxLayout* AssembledFirmwareLayout;
  QPlainTextEdit* AssembledFirmwareView;

 public:
  explicit ServerUserInterface(QWidget* parent = nullptr);
  ~ServerUserInterface() = default;

 private:
  void create(void);

  void createControlPanel(void);
  void createInitGroup(void);
  void createProductionLineGroup(void);
  void createBoxGroup(void);
  void createTransponderGroup(void);
  void createPrintingGroup(void);

  void createDataDisplayPanel(void);

  void connectDependencies(void);
};

#endif  // PERSOSERVERWIDGET_H
