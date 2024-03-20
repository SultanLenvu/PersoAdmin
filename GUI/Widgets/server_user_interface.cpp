#include "server_user_interface.h"
#include "assembly_unit_gui_subkernel.h"
#include "global_environment.h"
#include "server_connection_gui_subkernel.h"

ServerUserInterface::ServerUserInterface(QWidget* parent)
    : AbstractUserInterface{parent} {
  createWidgets();
  connectDependencies();
}

void ServerUserInterface::createWidgets() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  SubLayout = new QVBoxLayout();
  MainLayout->addLayout(SubLayout);

  createRawCommandGroup();
  createControlPanel();
  createDataDisplayPanel();

  // Настройка пропорции между объектами на основном макете
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void ServerUserInterface::createControlPanel() {
  ControlPanel = new QGroupBox("Команды сборочной единицы");
  SubLayout->addWidget(ControlPanel);

  ControlPanelLayout = new QVBoxLayout();
  ControlPanel->setLayout(ControlPanelLayout);

  createInitGroup();
  createProductionLineGroup();
  createBoxGroup();
  createTransponderGroup();
  createPrintingGroup();

  ControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  ControlPanelLayout->addItem(ControlPanelVS);
}

void ServerUserInterface::createRawCommandGroup() {
  RawCommandGroup = new QGroupBox("Клиентские запросы");
  SubLayout->addWidget(RawCommandGroup);

  RawCommandLayout = new QVBoxLayout();
  RawCommandGroup->setLayout(RawCommandLayout);

  CommandComboBox = new QComboBox();
  RawCommandLayout->addWidget(CommandComboBox);
  CommandComboBox->addItem("connect");
  CommandComboBox->addItem("disconnect");

  CommandComboBox->addItem("echo");
  CommandComboBox->addItem("launch_production_line");
  CommandComboBox->addItem("shutdown_production_line");
  CommandComboBox->addItem("get_production_line_data");

  CommandComboBox->addItem("request_box");
  CommandComboBox->addItem("get_current_box_data");
  CommandComboBox->addItem("refund_current_box");
  CommandComboBox->addItem("complete_current_box");

  CommandComboBox->addItem("release_transponder");
  CommandComboBox->addItem("confirm_transponder_release");
  CommandComboBox->addItem("rerelease_transponder");
  CommandComboBox->addItem("confirm_transponder_rerelease");
  CommandComboBox->addItem("rollback_tranponder");
  CommandComboBox->addItem("get_current_transponder_data");
  CommandComboBox->addItem("get_transponder_data");

  CommandComboBox->addItem("print_box_sticker");
  CommandComboBox->addItem("print_last_box_sticker");
  CommandComboBox->addItem("print_pallet_sticker");
  CommandComboBox->addItem("print_last_pallet_sticker");

  ExecuteCommandButton = new QPushButton("Выполнить");
  RawCommandLayout->addWidget(ExecuteCommandButton);
  connect(ExecuteCommandButton, &QPushButton::clicked, this,
          &ServerUserInterface::onExecuteCommandButton_slot);
}

void ServerUserInterface::createInitGroup() {
  InitGroup = new QGroupBox("Авторизация");
  ControlPanelLayout->addWidget(InitGroup);

  InitGroupLayout = new QVBoxLayout();
  InitGroup->setLayout(InitGroupLayout);

  LogOnPushButton = new QPushButton("Авторизироваться");
  InitGroupLayout->addWidget(LogOnPushButton);
  LogOutPushButton = new QPushButton("Выйти");
  InitGroupLayout->addWidget(LogOutPushButton);
}

void ServerUserInterface::createProductionLineGroup() {
  ProductionLineGroup = new QGroupBox("Производственная линия");
  ControlPanelLayout->addWidget(ProductionLineGroup);

  ProductionLineGroupLayout = new QVBoxLayout();
  ProductionLineGroup->setLayout(ProductionLineGroupLayout);

  LaunchProductionLinePushButton = new QPushButton("Запустить");
  ProductionLineGroupLayout->addWidget(LaunchProductionLinePushButton);
  ShutdownProductionLinePushButton = new QPushButton("Остановить");
  ProductionLineGroupLayout->addWidget(ShutdownProductionLinePushButton);
}

void ServerUserInterface::createBoxGroup() {
  BoxGroup = new QGroupBox("Боксы");
  ControlPanelLayout->addWidget(BoxGroup);

  BoxGroupLayout = new QVBoxLayout();
  BoxGroup->setLayout(BoxGroupLayout);

  RequestBoxPushButton = new QPushButton("Запросить");
  BoxGroupLayout->addWidget(RequestBoxPushButton);
  RefundBoxPushButton = new QPushButton("Вернуть");
  BoxGroupLayout->addWidget(RefundBoxPushButton);
  CompleteCurrentBoxPushButton = new QPushButton("Завершить сборку");
  BoxGroupLayout->addWidget(CompleteCurrentBoxPushButton);
}

void ServerUserInterface::createTransponderGroup() {
  TransponderGroup = new QGroupBox("Транспондеры");
  ControlPanelLayout->addWidget(TransponderGroup);

  TransponderGroupLayout = new QVBoxLayout();
  TransponderGroup->setLayout(TransponderGroupLayout);

  ReleaseTransponderPushButton = new QPushButton("Выпустить");
  TransponderGroupLayout->addWidget(ReleaseTransponderPushButton);
  RereleaseTransponderPushButton = new QPushButton("Перевыпустить");
  TransponderGroupLayout->addWidget(RereleaseTransponderPushButton);
  RollbackTransponderPushButton = new QPushButton("Откатить");
  TransponderGroupLayout->addWidget(RollbackTransponderPushButton);
}

void ServerUserInterface::createPrintingGroup() {
}

void ServerUserInterface::createDataDisplayPanel() {
  DataDisplayPanel = new QGroupBox("Данные");
  MainLayout->addWidget(DataDisplayPanel);

  DataDisplayLayout = new QGridLayout();
  DataDisplayPanel->setLayout(DataDisplayLayout);

  ProductionLineDataGroup = new QGroupBox("Производственная линия");
  DataDisplayLayout->addWidget(ProductionLineDataGroup, 0, 0, 1, 1);
  ProductionLineDataLayout = new QVBoxLayout();
  ProductionLineDataGroup->setLayout(ProductionLineDataLayout);
  ProductionLineDataView = new QTableView();
  ProductionLineDataLayout->addWidget(ProductionLineDataView);

  BoxDataGroup = new QGroupBox("Бокс");
  DataDisplayLayout->addWidget(BoxDataGroup, 0, 1, 1, 1);
  BoxDataLayout = new QVBoxLayout();
  BoxDataGroup->setLayout(BoxDataLayout);
  BoxDataView = new QTableView();
  BoxDataLayout->addWidget(BoxDataView);

  TransponderDataGroup = new QGroupBox("Транспондер");
  DataDisplayLayout->addWidget(TransponderDataGroup, 1, 0, 1, 1);
  TransponderDataLayout = new QVBoxLayout();
  TransponderDataGroup->setLayout(TransponderDataLayout);
  TransponderDataView = new QTableView();
  TransponderDataLayout->addWidget(TransponderDataView);

  FirmwareGroup = new QGroupBox("Прошивка");
  DataDisplayLayout->addWidget(FirmwareGroup, 1, 1, 1, 1);
  FirmwareLayout = new QVBoxLayout();
  FirmwareGroup->setLayout(FirmwareLayout);
  FirmwareView = new QListView();
  FirmwareLayout->addWidget(FirmwareView);
}

void ServerUserInterface::connectDependencies() {
  const ServerConnectionGuiSubkernel* sgs =
      GlobalEnvironment::instance()->getObject<const ServerConnectionGuiSubkernel>(
          "ServerConnectionGuiSubkernel");

  QObject::connect(this, &ServerUserInterface::executeCommand_signal, sgs,
                   &ServerConnectionGuiSubkernel::executeCommand);

  AssemblyUnitGuiSubkernel* augs =
      GlobalEnvironment::instance()->getObject<AssemblyUnitGuiSubkernel>(
          "AssemblyUnitGuiSubkernel");

  QObject::connect(LogOnPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::logOn);
  QObject::connect(LogOutPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::logOut);

  QObject::connect(RequestBoxPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::requestBox);
  QObject::connect(RefundBoxPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::refundCurrentBox);
  QObject::connect(CompleteCurrentBoxPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::completeCurrentBox);

  QObject::connect(ReleaseTransponderPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::releaseTransponder);
  QObject::connect(RereleaseTransponderPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::rereleaseTransponder);
  QObject::connect(RollbackTransponderPushButton, &QPushButton::clicked, augs,
                   &AssemblyUnitGuiSubkernel::rollbackTransponder);

  ProductionLineDataView->setModel(&augs->ProductionLineModel);
  BoxDataView->setModel(&augs->BoxModel);
  TransponderDataView->setModel(&augs->TransponderModel);
  FirmwareView->setModel(&augs->FirmwareModel);
}

void ServerUserInterface::onExecuteCommandButton_slot() {
  emit executeCommand_signal(CommandComboBox->currentText());
}
