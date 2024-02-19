#include "production_line_user_interface.h"
#include "global_environment.h"
#include "production_line_gui_subkernel.h"

ProductionLineUserInterface::ProductionLineUserInterface(QWidget* parent)
    : QWidget{parent} {
  create();
  connectDependencies();
}

ProductionLineUserInterface::~ProductionLineUserInterface() {}

void ProductionLineUserInterface::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayGroup();

  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void ProductionLineUserInterface::createControlPanel() {
  ControlPanel = new QGroupBox("Панель управления");
  MainLayout->addWidget(ControlPanel);

  ControlPanelLayout = new QVBoxLayout();
  ControlPanel->setLayout(ControlPanelLayout);

  CreatePushButton = new QPushButton("Создать производственную линию");
  ControlPanelLayout->addWidget(CreatePushButton);
  ActivatePushButton = new QPushButton("Запустить производственную линию");
  ControlPanelLayout->addWidget(ActivatePushButton);

  ActivateAllPushButton =
      new QPushButton("Запустить все производственные линии");
  ControlPanelLayout->addWidget(ActivateAllPushButton);
  DeactivatePushButton = new QPushButton("Остановить производственную линию");
  ControlPanelLayout->addWidget(DeactivatePushButton);
  DeactivateAllPushButton =
      new QPushButton("Остановить все производственные линии");
  ControlPanelLayout->addWidget(DeactivateAllPushButton);
  EditPushButton = new QPushButton("Редактировать производственную линию");
  ControlPanelLayout->addWidget(EditPushButton);

  ControlPanelVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ControlPanelLayout->addItem(ControlPanelVS1);

  GetPushButton = new QPushButton("Обновить таблицу");
  ControlPanelLayout->addWidget(GetPushButton);
}

void ProductionLineUserInterface::createDataDisplayGroup() {
  DataPanel = new QGroupBox("Данные производственных линий");
  MainLayout->addWidget(DataPanel);

  DataPanelLayout = new QVBoxLayout();
  DataPanel->setLayout(DataPanelLayout);

  DataTableView = new QTableView();
  DataPanelLayout->addWidget(DataTableView);
}

void ProductionLineUserInterface::connectDependencies() {
  ProductionLineGuiSubkernel* pls = static_cast<ProductionLineGuiSubkernel*>(
      GlobalEnvironment::instance()->getObject("ProductionLineGuiSubkernel"));

  connect(CreatePushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::create);
  connect(ActivatePushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::activate);
  connect(DeactivatePushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::deactivate);
  connect(ActivateAllPushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::activateAll);
  connect(DeactivateAllPushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::deactivateAll);
  connect(GetPushButton, &QPushButton::clicked, pls,
          &ProductionLineGuiSubkernel::get);

  DataTableView->setModel(pls->productionLines());
}
