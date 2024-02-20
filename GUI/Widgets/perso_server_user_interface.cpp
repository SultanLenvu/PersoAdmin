#include "perso_server_user_interface.h"
#include "global_environment.h"
#include "perso_server_gui_subkernel.h"

PersoServerUserInterface::PersoServerUserInterface(QWidget* parent) : QWidget{parent} {
  create();
  connectDependencies();
}

void PersoServerUserInterface::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayPanel();

  // Настройка пропорции между объектами на основном макете
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void PersoServerUserInterface::createControlPanel() {
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

void PersoServerUserInterface::createInitGroup() {
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

void PersoServerUserInterface::createProductionLineGroup() {
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

void PersoServerUserInterface::createBoxGroup() {
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

void PersoServerUserInterface::createTransponderGroup() {
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

void PersoServerUserInterface::createPrintingGroup() {
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

void PersoServerUserInterface::createDataDisplayPanel() {
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

void PersoServerUserInterface::connectDependencies() {
  PersoServerGuiSubkernel* psgs = static_cast<PersoServerGuiSubkernel*>(
      GlobalEnvironment::instance()->getObject("PersoServerGuiSubkernel"));

  QObject::connect(ConnectPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::connect);
  QObject::connect(DisconnectPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::disconnect);

  QObject::connect(EchoPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::echo);
  QObject::connect(LogOnPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::logOn);
  QObject::connect(LogOutPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::logOut);

  QObject::connect(LaunchProductionLinePushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::launchProductionLine);
  QObject::connect(ShutdownProductionLinePushButton, &QPushButton::clicked,
                   psgs, &PersoServerGuiSubkernel::shutdownProductionLine);
  QObject::connect(GetProductionLineDataPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::getProductionLineData);

  QObject::connect(RequestBoxPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::requestBox);
  QObject::connect(GetCurrentBoxDataPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::getCurrentBoxData);
  QObject::connect(RefundBoxPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::refundCurrentBox);
  QObject::connect(CompleteCurrentBoxPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::completeCurrentBox);

  QObject::connect(ReleaseTransponderPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::releaseTransponder);
  QObject::connect(RereleaseTransponderPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::rereleaseTransponder);
  QObject::connect(RollbackTransponderPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::rollbackTransponder);
  QObject::connect(GetCurrentTransponderDataPushButton, &QPushButton::clicked,
                   psgs, &PersoServerGuiSubkernel::getCurrentTransponderData);
  QObject::connect(GetTransponderDataPushButton, &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::getTransponderData);

  QObject::connect(PrintBoxStickerOnServerPushButton, &QPushButton::clicked,
                   psgs, &PersoServerGuiSubkernel::printBoxSticker);
  QObject::connect(PrintLastBoxStickerOnServerPushButton, &QPushButton::clicked,
                   psgs, &PersoServerGuiSubkernel::printLastBoxSticker);
  QObject::connect(PrintPalletStickerOnServerPushButton, &QPushButton::clicked,
                   psgs, &PersoServerGuiSubkernel::printPalletSticker);
  QObject::connect(PrintLastPalletStickerOnServerPushButton,
                   &QPushButton::clicked, psgs,
                   &PersoServerGuiSubkernel::printLastPalletSticker);

  ProductionLineDataView->setModel(&psgs->productionLine());
  BoxDataView->setModel(&psgs->box());
  TransponderDataView->setModel(&psgs->transponder());
  psgs->setFirmwareDisplay(AssembledFirmwareView);
}
