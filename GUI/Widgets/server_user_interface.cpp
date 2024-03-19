#include "server_user_interface.h"
#include "global_environment.h"
#include "server_gui_subkernel.h"

ServerUserInterface::ServerUserInterface(QWidget* parent)
    : AbstractUserInterface{parent} {
  create();
  connectDependencies();
}

void ServerUserInterface::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayPanel();

  // Настройка пропорции между объектами на основном макете
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void ServerUserInterface::createControlPanel() {
  ControlPanel = new QGroupBox("Панель управления");
  MainLayout->addWidget(ControlPanel);

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
  RawCommandGroup = new QGroupBox("Отдельные команды");
  ControlPanelLayout->addWidget(RawCommandGroup);

  RawCommandLayout = new QVBoxLayout();
  RawCommandGroup->setLayout(RawCommandLayout);

  CommandComboBox = new QComboBox();
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
  connect(ExecuteCommandButton, &QPushButton::clicked, this,
          &ServerUserInterface::onExecuteCommandButton_slot);
}

void ServerUserInterface::createInitGroup() {
  InitGroup = new QGroupBox("Авторизация");
  ControlPanelLayout->addWidget(InitGroup);

  InitGroupLayout = new QVBoxLayout();
  InitGroup->setLayout(InitGroupLayout);

  ConnectPushButton = new QPushButton("Подключиться");
  InitGroupLayout->addWidget(ConnectPushButton);
  DisconnectPushButton = new QPushButton("Отключиться");
  InitGroupLayout->addWidget(DisconnectPushButton);
  EchoPushButton = new QPushButton("Эхо-запрос");
  InitGroupLayout->addWidget(EchoPushButton);
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
  GetProductionLineDataPushButton = new QPushButton("Получить данные");
  ProductionLineGroupLayout->addWidget(GetProductionLineDataPushButton);
}

void ServerUserInterface::createBoxGroup() {
  BoxGroup = new QGroupBox("Боксы");
  ControlPanelLayout->addWidget(BoxGroup);

  BoxGroupLayout = new QVBoxLayout();
  BoxGroup->setLayout(BoxGroupLayout);

  RequestBoxPushButton = new QPushButton("Запросить");
  BoxGroupLayout->addWidget(RequestBoxPushButton);
  GetCurrentBoxDataPushButton = new QPushButton("Запросить текущие данные");
  BoxGroupLayout->addWidget(GetCurrentBoxDataPushButton);
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
  GetCurrentTransponderDataPushButton =
      new QPushButton("Запросить текущие данные");
  TransponderGroupLayout->addWidget(GetCurrentTransponderDataPushButton);
  GetTransponderDataPushButton = new QPushButton("Запросить данные");
  TransponderGroupLayout->addWidget(GetTransponderDataPushButton);
}

void ServerUserInterface::createPrintingGroup() {
  PrintingGroup = new QGroupBox("Принтер стикеров");
  ControlPanelLayout->addWidget(PrintingGroup);

  PrintingGroupLayout = new QVBoxLayout();
  PrintingGroup->setLayout(PrintingGroupLayout);

  PrintBoxStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для бокса");
  PrintingGroupLayout->addWidget(PrintBoxStickerOnServerPushButton);
  PrintLastBoxStickerOnServerPushButton =
      new QPushButton("Повторить последний стикер для бокса");
  PrintingGroupLayout->addWidget(PrintLastBoxStickerOnServerPushButton);
  PrintPalletStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для паллет");
  PrintingGroupLayout->addWidget(PrintPalletStickerOnServerPushButton);
  PrintLastPalletStickerOnServerPushButton =
      new QPushButton("Повторить последний стикер для паллеты");
  PrintingGroupLayout->addWidget(PrintLastPalletStickerOnServerPushButton);
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

  AssembledFirmwareGroup = new QGroupBox("Прошивка");
  DataDisplayLayout->addWidget(AssembledFirmwareGroup, 1, 1, 1, 1);
  AssembledFirmwareLayout = new QVBoxLayout();
  AssembledFirmwareGroup->setLayout(AssembledFirmwareLayout);
  AssembledFirmwareView = new QPlainTextEdit();
  AssembledFirmwareLayout->addWidget(AssembledFirmwareView);
}

void ServerUserInterface::connectDependencies() {
  ServerGuiSubkernel* sgs = static_cast<ServerGuiSubkernel*>(
      GlobalEnvironment::instance()->getObject("ServerGuiSubkernel"));

  QObject::connect(this, &ServerUserInterface::executeCommand_signal, sgs,
                   &ServerGuiSubkernel::executeCommand);

  QObject::connect(ConnectPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::connect);
  QObject::connect(DisconnectPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::disconnect);

  QObject::connect(EchoPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::echo);
  QObject::connect(LogOnPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::logOn);
  QObject::connect(LogOutPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::logOut);

  QObject::connect(LaunchProductionLinePushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::launchProductionLine);
  QObject::connect(ShutdownProductionLinePushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::shutdownProductionLine);
  QObject::connect(GetProductionLineDataPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::getProductionLineData);

  QObject::connect(RequestBoxPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::requestBox);
  QObject::connect(GetCurrentBoxDataPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::getCurrentBoxData);
  QObject::connect(RefundBoxPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::refundCurrentBox);
  QObject::connect(CompleteCurrentBoxPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::completeCurrentBox);

  QObject::connect(ReleaseTransponderPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::releaseTransponder);
  QObject::connect(RereleaseTransponderPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::rereleaseTransponder);
  QObject::connect(RollbackTransponderPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::rollbackTransponder);
  QObject::connect(GetCurrentTransponderDataPushButton, &QPushButton::clicked,
                   sgs, &ServerGuiSubkernel::getCurrentTransponderData);
  QObject::connect(GetTransponderDataPushButton, &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::getTransponderData);

  QObject::connect(PrintBoxStickerOnServerPushButton, &QPushButton::clicked,
                   sgs, &ServerGuiSubkernel::printBoxSticker);
  QObject::connect(PrintLastBoxStickerOnServerPushButton, &QPushButton::clicked,
                   sgs, &ServerGuiSubkernel::printLastBoxSticker);
  QObject::connect(PrintPalletStickerOnServerPushButton, &QPushButton::clicked,
                   sgs, &ServerGuiSubkernel::printPalletSticker);
  QObject::connect(PrintLastPalletStickerOnServerPushButton,
                   &QPushButton::clicked, sgs,
                   &ServerGuiSubkernel::printLastPalletSticker);

  ProductionLineDataView->setModel(&sgs->productionLine());
  BoxDataView->setModel(&sgs->box());
  TransponderDataView->setModel(&sgs->transponder());
  sgs->setFirmwareDisplay(AssembledFirmwareView);
}

void ServerUserInterface::onExecuteCommandButton_slot() {
  emit executeCommand_signal(CommandComboBox->currentText());
}
