#include "database_user_interface.h"
#include "database_gui_subkernel.h"
#include "global_environment.h"

DatabaseUserInterface::DatabaseUserInterface(QWidget* parent)
    : AbstractUserInterface{parent} {
  create();
  connectDependecies();
}

void DatabaseUserInterface::create()
{
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayGroup();

  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void DatabaseUserInterface::createControlPanel()
{
  ControlPanel = new QGroupBox("Панель управления");
  MainLayout->addWidget(ControlPanel);

  ControlPanelLayout = new QVBoxLayout();
  ControlPanel->setLayout(ControlPanelLayout);

  ConnectPushButton = new QPushButton("Подключиться");
  ControlPanelLayout->addWidget(ConnectPushButton);

  DisconnectPushButton = new QPushButton("Отключиться");
  ControlPanelLayout->addWidget(DisconnectPushButton);

  DatabaseControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ControlPanelLayout->addItem(DatabaseControlPanelVS);

  TableChoice = new QComboBox();
  TableChoice->addItem("production_lines");
  TableChoice->addItem("transponders");
  TableChoice->addItem("orders");
  TableChoice->addItem("issuers");
  TableChoice->addItem("boxes");
  TableChoice->addItem("pallets");
  TableChoice->addItem("transport_master_keys");
  TableChoice->addItem("commercial_master_keys");
  ControlPanelLayout->addWidget(TableChoice);

  GetTablePushButton = new QPushButton("Открыть таблицу");
  ControlPanelLayout->addWidget(GetTablePushButton);

  DatabaseControlPanelVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ControlPanelLayout->addItem(DatabaseControlPanelVS1);

  CustomRequestLineEdit = new QLineEdit();
  ControlPanelLayout->addWidget(CustomRequestLineEdit);

  ExecCustomRequestPushButton = new QPushButton("Отправить команду");
  ControlPanelLayout->addWidget(ExecCustomRequestPushButton);
}

void DatabaseUserInterface::createDataDisplayGroup() {
  DataDisplayGroup = new QGroupBox(QString("Полученные данные"));
  DataDisplayGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  MainLayout->addWidget(DataDisplayGroup);

  DataDisplayLayout = new QVBoxLayout();
  DataDisplayGroup->setLayout(DataDisplayLayout);

  RecordTableView = new QTableView();
  DataDisplayLayout->addWidget(RecordTableView);
}

void DatabaseUserInterface::connectDependecies() {
  DatabaseGuiSubkernel* dui = static_cast<DatabaseGuiSubkernel*>(
      GlobalEnvironment::instance()->getObject("DatabaseGuiSubkernel"));

  connect(ConnectPushButton, &QPushButton::clicked, dui,
          &DatabaseGuiSubkernel::connect);
  connect(DisconnectPushButton, &QPushButton::clicked, dui,
          &DatabaseGuiSubkernel::disconnect);
  connect(GetTablePushButton, &QPushButton::clicked, this,
          &DatabaseUserInterface::getTablePushButton_slot);
  connect(ExecCustomRequestPushButton, &QPushButton::clicked, dui,
          &DatabaseGuiSubkernel::execCustomRequest);

  RecordTableView->setModel(dui->responseModel());

  connect(this, &DatabaseUserInterface::getTable_signal, dui,
          &DatabaseGuiSubkernel::getTable);
}

void DatabaseUserInterface::getTablePushButton_slot() {
  emit getTable_signal(TableChoice->currentText());
}
