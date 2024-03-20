#include "order_user_interface.h"
#include "global_environment.h"
#include "order_gui_subkernel.h"

OrderUserInterface::OrderUserInterface(QWidget* parent)
    : AbstractUserInterface{parent} {
  create();
  connectDependencies();
}

void OrderUserInterface::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayGroup();

  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void OrderUserInterface::createControlPanel() {
  ControlPanel = new QGroupBox("Панель управления");
  MainLayout->addWidget(ControlPanel);

  ControlPanelLayout = new QVBoxLayout();
  ControlPanel->setLayout(ControlPanelLayout);

  createMainActions();
  createServiceActions();
}

void OrderUserInterface::createMainActions() {
  MainActions = new QGroupBox("Основные функции");
  ControlPanelLayout->addWidget(MainActions);

  MainActionsLayout = new QVBoxLayout();
  MainActions->setLayout(MainActionsLayout);

  GetPushButton = new QPushButton("Обновить");
  MainActionsLayout->addWidget(GetPushButton);
  CreatePushButton = new QPushButton("Создать новый заказ");
  MainActionsLayout->addWidget(CreatePushButton);
  StartAssemblingPushButton = new QPushButton("Запустить сборку заказа");
  MainActionsLayout->addWidget(StartAssemblingPushButton);
  StopAssemblingPushButton = new QPushButton("Остановить сборку заказа");
  MainActionsLayout->addWidget(StopAssemblingPushButton);

  MainActionsVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  MainActionsLayout->addItem(MainActionsVS);

  GenerateShipmentRegisterPushButton =
      new QPushButton("Сгенерировать реестр отгрузки");
  MainActionsLayout->addWidget(GenerateShipmentRegisterPushButton);
}

void OrderUserInterface::createServiceActions() {
  ServiceActions = new QGroupBox("Сервисные функции");
  ControlPanelLayout->addWidget(ServiceActions);

  ServiceActionsLayout = new QVBoxLayout();
  ServiceActions->setLayout(ServiceActionsLayout);

  ReleasePushButton = new QPushButton("Принудительный выпуск");
  ServiceActionsLayout->addWidget(ReleasePushButton);
  RefundPushButton = new QPushButton("Принудительный возврат");
  ServiceActionsLayout->addWidget(RefundPushButton);
  InitTransportMasterKeysPushButton =
      new QPushButton("Инициализировать транспортные ключи");
  ServiceActionsLayout->addWidget(InitTransportMasterKeysPushButton);
  InitIssuersPushButton = new QPushButton("Инициализировать эмитентов");
  ServiceActionsLayout->addWidget(InitIssuersPushButton);
  LinkIssuerWithKeysPushButton = new QPushButton("Связать эмитента и ключи");
  ServiceActionsLayout->addWidget(LinkIssuerWithKeysPushButton);
}

void OrderUserInterface::createDataDisplayGroup() {
  OrderTablePanel = new QGroupBox("Таблица заказов");
  MainLayout->addWidget(OrderTablePanel);

  OrderTablePanelLayout = new QVBoxLayout();
  OrderTablePanel->setLayout(OrderTablePanelLayout);

  OrderTableView = new QTableView();
  OrderTablePanelLayout->addWidget(OrderTableView);
}

void OrderUserInterface::connectDependencies() {
  OrderGuiSubkernel* ogs =
      GlobalEnvironment::instance()->getObject<OrderGuiSubkernel>(
          "OrderGuiSubkernel");

  connect(GetPushButton, &QPushButton::clicked, ogs, &OrderGuiSubkernel::get);
  connect(CreatePushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::create);
  connect(StartAssemblingPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::startAssembling);
  connect(StopAssemblingPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::stopAssembling);
  connect(GenerateShipmentRegisterPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::generateShipmentRegister);

  connect(ReleasePushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::release);
  connect(RefundPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::refund);
  connect(InitIssuersPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::initIssuers);
  connect(InitTransportMasterKeysPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::initTransportMasterKeys);
  connect(LinkIssuerWithKeysPushButton, &QPushButton::clicked, ogs,
          &OrderGuiSubkernel::linkIssuerWithKeys);

  OrderTableView->setModel(&ogs->orders());
}
