#include "gui_master.h"

MasterGUI::MasterGUI(QWidget* parent) : GUI(parent, Master) {
  setObjectName("MasterGUI");
}

void MasterGUI::create() {
  // Создаем вкладки мастер меню
  createTabs();

  // Создаем виджеты логов
  createLog();

  // Настраиваем пропорции отображаемых элементов
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 1);
}

void MasterGUI::update() {
  DatabaseRandomModelView->resizeColumnsToContents();
  DatabaseRandomModelView->update();

  OrderTableView->resizeColumnsToContents();
  OrderTableView->update();

  ProductionLineTableView->resizeColumnsToContents();
  ProductionLineTableView->update();

  TransponderSeedTableView->resizeColumnsToContents();
  TransponderSeedTableView->update();

  IssuerTableView->resizeColumnsToContents();
  IssuerTableView->update();

  IssuerTableView->resizeColumnsToContents();
  IssuerTableView->update();
}

void MasterGUI::createTabs() {
  Tabs = new QTabWidget();
  MainLayout->addWidget(Tabs);

  // Задаем стартовую страницу
  Tabs->setCurrentIndex(0);

  // Конструируем вкладку для управления сервером
  createServerTab();

  // Контруируем вкладки для взаимодействия с базой данных
  createDatabaseTab();
  createOrderTab();
  createProductionLineTab();
  createTransponderTab();
  createIssuerTab();

  // Конструируем вкладку настроек
  createSettingsTab();
}

void MasterGUI::createServerTab() {
  ServerTab = new QWidget();
  Tabs->addTab(ServerTab, "Сервер");

  // Основной макет
  ServerMainLayout = new QHBoxLayout();
  ServerTab->setLayout(ServerMainLayout);

  // Панель управления
  ServerControlPanelGroup = new QGroupBox(QString("Панель управления"));
  ServerControlPanelGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ServerMainLayout->addWidget(ServerControlPanelGroup);

  ServerControlPanelLayout = new QVBoxLayout();
  ServerControlPanelGroup->setLayout(ServerControlPanelLayout);

  // Кнопки
  GetStatisticPushButton = new QPushButton("Запрос статистики");
  ServerControlPanelLayout->addWidget(GetStatisticPushButton);

  SendSetupPushButton = new QPushButton("Отправить настройки");
  ServerControlPanelLayout->addWidget(SendSetupPushButton);

  ServerControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ServerControlPanelLayout->addItem(ServerControlPanelVS);

  // Отображение графика нагрузки на сервер
  ServerChartGroup = new QGroupBox(QString("Нагрузка на сервер"));
  ServerChartGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  ServerMainLayout->addWidget(ServerChartGroup);

  ServerChartLayout = new QVBoxLayout();
  ServerChartGroup->setLayout(ServerChartLayout);

  ServerChartView = new QChartView();
  ServerChartLayout->addWidget(ServerChartView);

  // Настройка пропорции между объектами на макете
  ServerMainLayout->setStretch(0, 1);
  ServerMainLayout->setStretch(1, 3);
}

void MasterGUI::createDatabaseTab() {
  DatabaseTab = new QWidget();
  Tabs->addTab(DatabaseTab, "База данных");

  // Основной макет
  DatabaseMainLayout = new QHBoxLayout();
  DatabaseTab->setLayout(DatabaseMainLayout);

  // Панель управления
  DatabaseControlPanelGroup = new QGroupBox(QString("Панель управления"));
  DatabaseControlPanelGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  DatabaseMainLayout->addWidget(DatabaseControlPanelGroup);

  DatabaseControlPanelLayout = new QVBoxLayout();
  DatabaseControlPanelGroup->setLayout(DatabaseControlPanelLayout);

  // Кнопки
  ConnectDatabasePushButton = new QPushButton("Подключиться");
  DatabaseControlPanelLayout->addWidget(ConnectDatabasePushButton);

  DisconnectDatabasePushButton = new QPushButton("Отключиться");
  DatabaseControlPanelLayout->addWidget(DisconnectDatabasePushButton);

  DatabaseControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  DatabaseControlPanelLayout->addItem(DatabaseControlPanelVS);

  DatabaseTableChoice = new QComboBox();
  DatabaseTableChoice->addItem("production_lines");
  DatabaseTableChoice->addItem("transponders");
  DatabaseTableChoice->addItem("orders");
  DatabaseTableChoice->addItem("issuers");
  DatabaseTableChoice->addItem("boxes");
  DatabaseTableChoice->addItem("pallets");
  DatabaseTableChoice->addItem("transport_master_keys");
  DatabaseTableChoice->addItem("commercial_master_keys");
  DatabaseControlPanelLayout->addWidget(DatabaseTableChoice);

  ShowDatabaseTablePushButton = new QPushButton("Открыть таблицу");
  DatabaseControlPanelLayout->addWidget(ShowDatabaseTablePushButton);
  ClearDatabaseTablePushButton = new QPushButton("Очистить таблицу");
  DatabaseControlPanelLayout->addWidget(ClearDatabaseTablePushButton);

  DatabaseControlPanelVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  DatabaseControlPanelLayout->addItem(DatabaseControlPanelVS1);

  CustomRequestLineEdit = new QLineEdit();
  DatabaseControlPanelLayout->addWidget(CustomRequestLineEdit);

  TransmitCustomRequestPushButton = new QPushButton("Отправить команду");
  DatabaseControlPanelLayout->addWidget(TransmitCustomRequestPushButton);

  // Отображение буфера считанных данных из БД
  DatabaseBufferGroup = new QGroupBox(QString("Буффер считанных данных"));
  DatabaseBufferGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  DatabaseMainLayout->addWidget(DatabaseBufferGroup);

  DatabaseBufferLayout = new QVBoxLayout();
  DatabaseBufferGroup->setLayout(DatabaseBufferLayout);

  DatabaseRandomModelView = new QTableView();
  DatabaseBufferLayout->addWidget(DatabaseRandomModelView);

  // Настройка пропорции между объектами на макете
  DatabaseMainLayout->setStretch(0, 1);
  DatabaseMainLayout->setStretch(1, 3);
}

void MasterGUI::createOrderTab() {
  OrderTab = new QWidget();
  Tabs->addTab(OrderTab, "Создание заказов");

  // Основной макет
  OrderTabMainLayout = new QHBoxLayout();
  OrderTab->setLayout(OrderTabMainLayout);

  // Панель для создания
  OrderControlPanel = new QGroupBox("Панель управления");
  OrderTabMainLayout->addWidget(OrderControlPanel);

  OrderControlPanelLayout = new QVBoxLayout();
  OrderControlPanel->setLayout(OrderControlPanelLayout);

  FullPersonalizationCheckBox = new QCheckBox("Полная персонализация");
  OrderControlPanelLayout->addWidget(FullPersonalizationCheckBox);

  OrderPanelSubLayout = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout);
  PanFilePathLabel = new QLabel("PAN-файл");
  OrderPanelSubLayout->addWidget(PanFilePathLabel);
  PanFilePathLineEdit = new QLineEdit();
  OrderPanelSubLayout->addWidget(PanFilePathLineEdit);
  PanFileExplorePushButton = new QPushButton("Обзор");
  OrderPanelSubLayout->addWidget(PanFileExplorePushButton);
  connect(PanFileExplorePushButton, &QPushButton::clicked, this,
          &MasterGUI::on_PanFileExplorePushButton_slot);

  OrderPanelSubLayout1 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout1);

  IssuerNameComboLabel = new QLabel("Компания заказчик");
  OrderPanelSubLayout1->addWidget(IssuerNameComboLabel);

  IssuerNameComboBox = new QComboBox();
  IssuerNameComboBox->addItem("Новое качество дорог");
  IssuerNameComboBox->addItem("Западный скоростной диаметр");
  OrderPanelSubLayout1->addWidget(IssuerNameComboBox);

  OrderPanelSubLayout2 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout2);
  TransponderQuantityLabel = new QLabel("Количество транспондеров");
  OrderPanelSubLayout2->addWidget(TransponderQuantityLabel);
  TransponderQuantityLineEdit = new QLineEdit("500");
  OrderPanelSubLayout2->addWidget(TransponderQuantityLineEdit);

  OrderPanelSubLayout3 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout3);
  BoxCapacityLabel = new QLabel("Емкость бокса");
  OrderPanelSubLayout3->addWidget(BoxCapacityLabel);
  BoxCapacityLineEdit = new QLineEdit("50");
  OrderPanelSubLayout3->addWidget(BoxCapacityLineEdit);

  OrderPanelSublayout4 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSublayout4);
  PalletCapacityLabel = new QLabel("Емкость палеты");
  OrderPanelSublayout4->addWidget(PalletCapacityLabel);
  PalletCapacityLineEdit = new QLineEdit("10");
  OrderPanelSublayout4->addWidget(PalletCapacityLineEdit);

  OrderPanelSublayout5 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSublayout5);
  TransponderModelLabel = new QLabel("Модель транспондера");
  OrderPanelSublayout5->addWidget(TransponderModelLabel);
  TransponderModelLineEdit = new QLineEdit(" TC1001");
  OrderPanelSublayout5->addWidget(TransponderModelLineEdit);

  OrderPanelSubLayout6 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout6);
  AccrReferenceLabel = new QLabel("ACCR Reference (HEX)");
  OrderPanelSubLayout6->addWidget(AccrReferenceLabel);
  AccrReferenceLineEdit = new QLineEdit("1DD1");
  OrderPanelSubLayout6->addWidget(AccrReferenceLineEdit);

  OrderPanelSubLayout7 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout7);
  EquipmentClassLabel = new QLabel("Класс оборудования (HEX)");
  OrderPanelSubLayout7->addWidget(EquipmentClassLabel);
  EquipmentClassLineEdit = new QLineEdit("F301");
  OrderPanelSubLayout7->addWidget(EquipmentClassLineEdit);

  OrderPanelSubLayout8 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderPanelSubLayout8);
  ManufacturerIdLabel = new QLabel("Идентификатор производителя (HEX)");
  OrderPanelSubLayout8->addWidget(ManufacturerIdLabel);
  ManufacturerIdLineEdit = new QLineEdit("0032");
  OrderPanelSubLayout8->addWidget(ManufacturerIdLineEdit);

  CreateNewOrderPushButton = new QPushButton("Создать новый заказ");
  OrderControlPanelLayout->addWidget(CreateNewOrderPushButton);

  OrderControlPanelVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  OrderControlPanelLayout->addItem(OrderControlPanelVS1);

  OrderIdLayout1 = new QHBoxLayout();
  OrderControlPanelLayout->addLayout(OrderIdLayout1);
  OrderIdLabel1 = new QLabel("ID заказа: ");
  OrderIdLayout1->addWidget(OrderIdLabel1);
  OrderIdLineEdit1 = new QLineEdit();
  OrderIdLayout1->addWidget(OrderIdLineEdit1);
  StartOrderAssemblingPushButton = new QPushButton("Начать сборку заказа");
  OrderControlPanelLayout->addWidget(StartOrderAssemblingPushButton);
  StopOrderAssemblingPushButton = new QPushButton("Остановить сборку заказа");
  OrderControlPanelLayout->addWidget(StopOrderAssemblingPushButton);

  OrderControlPanelVS2 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  OrderControlPanelLayout->addItem(OrderControlPanelVS2);

  UpdateOrderViewPushButton = new QPushButton("Обновить таблицу");
  OrderControlPanelLayout->addWidget(UpdateOrderViewPushButton);

  DeleteLastOrderPushButton =
      new QPushButton("Удалить последний созданный заказ");
  OrderControlPanelLayout->addWidget(DeleteLastOrderPushButton);

  // Панель для отображения таблицы заказов
  OrderTablePanel = new QGroupBox("Таблица заказов");
  OrderTabMainLayout->addWidget(OrderTablePanel);

  OrderTablePanelLayout = new QVBoxLayout();
  OrderTablePanel->setLayout(OrderTablePanelLayout);

  OrderTableView = new QTableView();
  OrderTablePanelLayout->addWidget(OrderTableView);

  // Настройка пропорции между объектами на макете
  OrderTabMainLayout->setStretch(0, 1);
  OrderTabMainLayout->setStretch(1, 3);
}

void MasterGUI::createProductionLineTab() {
  ProductionLinesTab = new QWidget();
  Tabs->addTab(ProductionLinesTab, "Линии производства");

  ProductionLinesTabMainLayout = new QHBoxLayout();
  ProductionLinesTab->setLayout(ProductionLinesTabMainLayout);

  // Панель управления
  ProductionLinesControlPanel = new QGroupBox("Панель управления");
  ProductionLinesTabMainLayout->addWidget(ProductionLinesControlPanel);

  ProductionLinesControlPanelLayout = new QVBoxLayout();
  ProductionLinesControlPanel->setLayout(ProductionLinesControlPanelLayout);

  LoginLayout1 = new QHBoxLayout();
  ProductionLinesControlPanelLayout->addLayout(LoginLayout1);
  LoginLabel1 = new QLabel("Логин: ");
  LoginLayout1->addWidget(LoginLabel1);
  LoginLineEdit1 = new QLineEdit();
  LoginLayout1->addWidget(LoginLineEdit1);
  PasswordLayout1 = new QHBoxLayout();
  ProductionLinesControlPanelLayout->addLayout(PasswordLayout1);
  PasswordLabel1 = new QLabel("Пароль: ");
  PasswordLayout1->addWidget(PasswordLabel1);
  PasswordLineEdit1 = new QLineEdit();
  PasswordLayout1->addWidget(PasswordLineEdit1);
  CreateNewProductionLinePushButton =
      new QPushButton("Создать новую производственную линию");
  ProductionLinesControlPanelLayout->addWidget(
      CreateNewProductionLinePushButton);

  ProductionLinesControlPanelVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ProductionLinesControlPanelLayout->addItem(ProductionLinesControlPanelVS1);

  OrderIdLayout2 = new QHBoxLayout();
  ProductionLinesControlPanelLayout->addLayout(OrderIdLayout2);
  OrderIdLabel2 = new QLabel("ID заказа: ");
  OrderIdLayout2->addWidget(OrderIdLabel2);
  OrderIdLineEdit2 = new QLineEdit();
  OrderIdLayout2->addWidget(OrderIdLineEdit2);
  AllocateInactiveProductionLinesPushButton =
      new QPushButton("Распределить неактивные производственные линии");
  ProductionLinesControlPanelLayout->addWidget(
      AllocateInactiveProductionLinesPushButton);

  ProductionLinesControlPanelVS2 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ProductionLinesControlPanelLayout->addItem(ProductionLinesControlPanelVS2);

  BoxIdLayout = new QHBoxLayout();
  ProductionLinesControlPanelLayout->addLayout(BoxIdLayout);
  BoxIdLabel = new QLabel("ID бокса: ");
  BoxIdLayout->addWidget(BoxIdLabel);
  BoxIdLineEdit = new QLineEdit();
  BoxIdLayout->addWidget(BoxIdLineEdit);
  LinkProductionLinePushButton =
      new QPushButton("Связать с производственной линией");
  ProductionLinesControlPanelLayout->addWidget(LinkProductionLinePushButton);

  // Сжатие по вертикали
  ProductionLinesControlPanelVS3 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ProductionLinesControlPanelLayout->addItem(ProductionLinesControlPanelVS3);

  DeactivateAllProductionLinesPushButton =
      new QPushButton("Остановить все производственные линии");
  ProductionLinesControlPanelLayout->addWidget(
      DeactivateAllProductionLinesPushButton);
  UpdateProductionLineViewPushButton = new QPushButton("Обновить таблицу");
  ProductionLinesControlPanelLayout->addWidget(
      UpdateProductionLineViewPushButton);

  DeleteLastProductionLinePushButton =
      new QPushButton("Удалить последнюю созданную линию производства");
  ProductionLinesControlPanelLayout->addWidget(
      DeleteLastProductionLinePushButton);

  // Панель для отображения таблицы производственных линий
  ProductionLineTablePanel = new QGroupBox("Таблица производственных линий");
  ProductionLinesTabMainLayout->addWidget(ProductionLineTablePanel);

  ProductionLineTableLayout = new QVBoxLayout();
  ProductionLineTablePanel->setLayout(ProductionLineTableLayout);

  ProductionLineTableView = new QTableView();
  ProductionLineTableLayout->addWidget(ProductionLineTableView);

  // Настройка пропорции между объектами на основном макете
  ProductionLinesTabMainLayout->setStretch(0, 1);
  ProductionLinesTabMainLayout->setStretch(1, 3);
}

void MasterGUI::createTransponderTab() {
  TransponderTab = new QWidget();
  Tabs->addTab(TransponderTab, "Выпуск транспондеров");

  TransponderTabMainLayout = new QHBoxLayout();
  TransponderTab->setLayout(TransponderTabMainLayout);

  // Панель управления
  TransponderControlPanel = new QGroupBox("Панель управления");
  TransponderTabMainLayout->addWidget(TransponderControlPanel);

  TransponderControlPanelLayout = new QVBoxLayout();
  TransponderControlPanel->setLayout(TransponderControlPanelLayout);

  LoginLayout2 = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(LoginLayout2);
  LoginLabel2 = new QLabel("Логин: ");
  LoginLayout2->addWidget(LoginLabel2);
  LoginLineEdit2 = new QLineEdit();
  LoginLineEdit2->setText("1");
  LoginLayout2->addWidget(LoginLineEdit2);

  PasswordLayout2 = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(PasswordLayout2);
  PasswordLabel2 = new QLabel("Пароль: ");
  PasswordLayout2->addWidget(PasswordLabel2);
  PasswordLineEdit2 = new QLineEdit();
  PasswordLineEdit2->setText("1");
  PasswordLayout2->addWidget(PasswordLineEdit2);

  UcidLayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(UcidLayout);
  UcidLabel = new QLabel("UCID: ");
  UcidLayout->addWidget(UcidLabel);
  UcidLineEdit = new QLineEdit();
  UcidLineEdit->setMaxLength(UCID_CHAR_LENGTH);
  UcidLineEdit->setText("11111111111111111111111111111111");
  UcidLayout->addWidget(UcidLineEdit);

  ReleaseTransponderPushButton = new QPushButton("Выпустить");
  TransponderControlPanelLayout->addWidget(ReleaseTransponderPushButton);
  ConfirmTransponderPushButton = new QPushButton("Подтвердить");
  TransponderControlPanelLayout->addWidget(ConfirmTransponderPushButton);

  TransponderControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  TransponderControlPanelLayout->addItem(TransponderControlPanelVS);

  SearchTransponderByLayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(SearchTransponderByLayout);
  SearchTransponderByComboBox = new QComboBox();
  SearchTransponderByComboBox->addItem("SN");
  SearchTransponderByComboBox->addItem("UCID");
  SearchTransponderByComboBox->addItem("PAN");
  SearchTransponderByComboBox->setCurrentIndex(0);
  SearchTransponderByLayout->addWidget(SearchTransponderByComboBox);
  connect(SearchTransponderByComboBox, &QComboBox::currentTextChanged, this,
          &MasterGUI::on_SearchTransponderByComboBox_slot);
  SearchTransponderLineEdit = new QLineEdit();
  SearchTransponderLineEdit->setMaxLength(
      TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  SearchTransponderByLayout->addWidget(SearchTransponderLineEdit);

  SearchTransponderPushButton = new QPushButton("Найти");
  TransponderControlPanelLayout->addWidget(SearchTransponderPushButton);
  RefundTransponderPushButton = new QPushButton("Отозвать");
  TransponderControlPanelLayout->addWidget(RefundTransponderPushButton);

  LoginLayout3 = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(LoginLayout3);
  LoginLabel3 = new QLabel("Логин: ");
  LoginLayout3->addWidget(LoginLabel3);
  LoginLineEdit3 = new QLineEdit();
  LoginLineEdit3->setText("1");
  LoginLayout3->addWidget(LoginLineEdit3);

  PasswordLayout3 = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(PasswordLayout3);
  PasswordLabel3 = new QLabel("Пароль: ");
  PasswordLayout3->addWidget(PasswordLabel3);
  PasswordLineEdit3 = new QLineEdit();
  PasswordLineEdit3->setText("1");
  PasswordLayout3->addWidget(PasswordLineEdit3);

  RereleaseTransponderLayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(RereleaseTransponderLayout);
  RereleaseTransponderByComboBox = new QComboBox();
  RereleaseTransponderByComboBox->addItem("SN");
  RereleaseTransponderByComboBox->addItem("PAN");
  RereleaseTransponderByComboBox->setCurrentIndex(0);
  RereleaseTransponderLayout->addWidget(RereleaseTransponderByComboBox);
  connect(RereleaseTransponderByComboBox, &QComboBox::currentTextChanged, this,
          &MasterGUI::on_RereleaseTransponderByComboBox_slot);
  RereleaseTransponderLineEdit = new QLineEdit();
  RereleaseTransponderLineEdit->setMaxLength(
      TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  RereleaseTransponderLayout->addWidget(RereleaseTransponderLineEdit);

  NewUcidLayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(NewUcidLayout);
  NewUcidLabel = new QLabel("UCID:");
  NewUcidLayout->addWidget(NewUcidLabel);
  NewUcidLineEdit = new QLineEdit();
  NewUcidLineEdit->setMaxLength(UCID_CHAR_LENGTH);
  NewUcidLayout->addWidget(NewUcidLineEdit);

  RereleaseTransponderPushButton = new QPushButton("Перевыпустить");
  TransponderControlPanelLayout->addWidget(RereleaseTransponderPushButton);
  ConfirmRereleaseTransponderPushButton =
      new QPushButton("Подтвердить перевыпуск");
  TransponderControlPanelLayout->addWidget(
      ConfirmRereleaseTransponderPushButton);

  // Панель отображения
  TransponderDisplayPanel = new QGroupBox("Данные транспондера");
  TransponderTabMainLayout->addWidget(TransponderDisplayPanel);

  TransponderDisplayLayout = new QVBoxLayout();
  TransponderDisplayPanel->setLayout(TransponderDisplayLayout);

  TransponderSeedTableView = new QTableView();
  TransponderDisplayLayout->addWidget(TransponderSeedTableView);

  AssembledFirmwareView = new QPlainTextEdit();
  TransponderDisplayLayout->addWidget(AssembledFirmwareView);

  // Настройка пропорции между объектами на основном макете
  TransponderTabMainLayout->setStretch(0, 1);
  TransponderTabMainLayout->setStretch(1, 3);
}

void MasterGUI::createIssuerTab() {
  IssuerTab = new QWidget();
  Tabs->addTab(IssuerTab, "Эмитенты");

  // Основной макет
  IssuerTabMainLayout = new QHBoxLayout();
  IssuerTab->setLayout(IssuerTabMainLayout);

  // Панель управления
  IssuerControlPanelGroup = new QGroupBox(QString("Панель управления"));
  IssuerControlPanelGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  IssuerTabMainLayout->addWidget(IssuerControlPanelGroup);

  IssuerControlPanelLayout = new QVBoxLayout();
  IssuerControlPanelGroup->setLayout(IssuerControlPanelLayout);

  IssuerTableChoice = new QComboBox();
  IssuerTableChoice->addItem("Заказчики");
  IssuerTableChoice->addItem("Транспортные мастер ключи");
  IssuerTableChoice->addItem("Коммерческие мастер ключи");
  IssuerTableChoice->setCurrentIndex(0);
  IssuerControlPanelLayout->addWidget(IssuerTableChoice);

  ShowIssuerTablePushButton = new QPushButton("Загрузить");
  IssuerControlPanelLayout->addWidget(ShowIssuerTablePushButton);
  InitTransportMasterKeysPushButton =
      new QPushButton("Инициализировать транспортные ключи");
  IssuerControlPanelLayout->addWidget(InitTransportMasterKeysPushButton);
  InitIssuerTablePushButton = new QPushButton("Инициализировать эмитентов");
  IssuerControlPanelLayout->addWidget(InitIssuerTablePushButton);

  TransportKeyVS1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  IssuerControlPanelLayout->addItem(TransportKeyVS1);

  IssuerIdLayout1 = new QHBoxLayout();
  IssuerControlPanelLayout->addLayout(IssuerIdLayout1);
  IssuerIdLabel1 = new QLabel("ID эмитента: ");
  IssuerIdLayout1->addWidget(IssuerIdLabel1);
  IssuerIdLineEdit1 = new QLineEdit();
  IssuerIdLayout1->addWidget(IssuerIdLineEdit1);

  MasterKeysIdLayout1 = new QHBoxLayout();
  IssuerControlPanelLayout->addLayout(MasterKeysIdLayout1);
  MasterKeysChoice = new QComboBox();
  MasterKeysChoice->addItem("Транспортные мастер ключи");
  MasterKeysChoice->addItem("Коммерческие мастер ключи");
  MasterKeysChoice->setCurrentIndex(0);
  MasterKeysIdLayout1->addWidget(MasterKeysChoice);
  MasterKeysLineEdit1 = new QLineEdit();
  MasterKeysIdLayout1->addWidget(MasterKeysLineEdit1);

  LinkIssuerWithKeysPushButton = new QPushButton("Связать эмитента и ключи");
  IssuerControlPanelLayout->addWidget(LinkIssuerWithKeysPushButton);

  // Отображение буфера считанных данных из БД
  IssuerViewGroup = new QGroupBox(QString("Таблица"));
  IssuerViewGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  IssuerTabMainLayout->addWidget(IssuerViewGroup);

  IssuerTableViewLayout = new QVBoxLayout();
  IssuerViewGroup->setLayout(IssuerTableViewLayout);

  IssuerTableView = new QTableView();
  IssuerTableViewLayout->addWidget(IssuerTableView);

  // Настройка пропорции между объектами на макете
  IssuerTabMainLayout->setStretch(0, 1);
  IssuerTabMainLayout->setStretch(1, 3);
}

void MasterGUI::createLog() {
  LogGroup = new QGroupBox("Лог");
  LogGroup->setAlignment(Qt::AlignCenter);
  MainLayout->addWidget(LogGroup);

  LogLayout = new QVBoxLayout();
  LogGroup->setLayout(LogLayout);

  LogDisplay = new QPlainTextEdit();
  LogDisplay = new QPlainTextEdit();
  LogDisplay->setEnabled(true);
  LogDisplay->setTabletTracking(true);
  LogDisplay->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  LogDisplay->setCenterOnScroll(false);
  LogLayout->addWidget(LogDisplay);
}

void MasterGUI::createSettingsTab() {
  QSettings settings;
  SettingsTab = new QWidget();
  Tabs->addTab(SettingsTab, "Настройки");

  // Главный макет меню настроек
  SettingsMainLayout = new QHBoxLayout();
  SettingsTab->setLayout(SettingsMainLayout);

  SettingsMainSubLayout = new QVBoxLayout();
  SettingsMainLayout->addLayout(SettingsMainSubLayout);

  SettingsHorizontalSpacer1 =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
  SettingsMainLayout->addItem(SettingsHorizontalSpacer1);

  // Настройки системы адиминистрирования
  AdministrationSystemSettingsGroupBox =
      new QGroupBox(QString("Настройки системы администрирования"));
  SettingsMainSubLayout->addWidget(AdministrationSystemSettingsGroupBox);
  AdministrationSystemSettingsLayout = new QGridLayout();
  AdministrationSystemSettingsGroupBox->setLayout(
      AdministrationSystemSettingsLayout);

  AdministrationSystemLogEnableLabel = new QLabel("Логирование ");
  AdministrationSystemSettingsLayout->addWidget(
      AdministrationSystemLogEnableLabel, 0, 0, 1, 1);
  AdministrationSystemLogEnable = new QCheckBox();
  AdministrationSystemLogEnable->setCheckState(
      settings.value("AdministrationSystem/LogEnable").toBool()
          ? Qt::Checked
          : Qt::Unchecked);
  AdministrationSystemSettingsLayout->addWidget(AdministrationSystemLogEnable,
                                                0, 1, 1, 1);

  // Настройки системы взаимодействия с пользователем
  UserInteractionSystemSettingsGroupBox = new QGroupBox(
      QString("Настройки системы взаимодействия с пользователем"));
  SettingsMainSubLayout->addWidget(UserInteractionSystemSettingsGroupBox);
  UserInteractionSystemSettingsLayout = new QGridLayout();
  UserInteractionSystemSettingsGroupBox->setLayout(
      UserInteractionSystemSettingsLayout);

  UserInteractionSystemLogEnableLabel = new QLabel("Логирование ");
  UserInteractionSystemSettingsLayout->addWidget(
      UserInteractionSystemLogEnableLabel, 0, 0, 1, 1);
  UserInteractionSystemLogEnable = new QCheckBox();
  UserInteractionSystemLogEnable->setCheckState(
      settings.value("UserInteractionSystem/LogEnable").toBool()
          ? Qt::Checked
          : Qt::Unchecked);
  UserInteractionSystemSettingsLayout->addWidget(UserInteractionSystemLogEnable,
                                                 0, 1, 1, 1);

  // Настройки базы данных
  DatabaseSettingsGroupBox = new QGroupBox(QString("Настройки базы данных"));
  SettingsMainSubLayout->addWidget(DatabaseSettingsGroupBox);

  DatabaseSettingsLayout = new QGridLayout();
  DatabaseSettingsGroupBox->setLayout(DatabaseSettingsLayout);

  DatabaseIpLabel = new QLabel("IP-адрес");
  DatabaseSettingsLayout->addWidget(DatabaseIpLabel, 0, 0, 1, 1);

  DatabaseIpLineEdit =
      new QLineEdit(settings.value("PostgresController/Server/Ip").toString());
  DatabaseSettingsLayout->addWidget(DatabaseIpLineEdit, 0, 1, 1, 1);

  DatabasePortLabel = new QLabel("Порт ");
  DatabaseSettingsLayout->addWidget(DatabasePortLabel, 1, 0, 1, 1);

  DatabasePortLineEdit = new QLineEdit(
      settings.value("PostgresController/Server/Port").toString());
  DatabaseSettingsLayout->addWidget(DatabasePortLineEdit, 1, 1, 1, 1);

  DatabaseNameLabel = new QLabel("Название базы данных ");
  DatabaseSettingsLayout->addWidget(DatabaseNameLabel, 2, 0, 1, 1);

  DatabaseNameLineEdit = new QLineEdit(
      settings.value("PostgresController/Database/Name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseNameLineEdit, 2, 1, 1, 1);

  DatabaseUserNameLabel = new QLabel("Имя пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLabel, 3, 0, 1, 1);

  DatabaseUserNameLineEdit =
      new QLineEdit(settings.value("PostgresController/User/Name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLineEdit, 3, 1, 1, 1);

  DatabaseUserPasswordLabel = new QLabel("Пароль пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLabel, 4, 0, 1, 1);

  DatabaseUserPasswordLineEdit = new QLineEdit(
      settings.value("PostgresController/User/Password").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLineEdit, 4, 1, 1, 1);

  IDatabaseControllerLogEnableLabel = new QLabel("Логирование ");
  DatabaseSettingsLayout->addWidget(IDatabaseControllerLogEnableLabel, 5, 0, 1,
                                    1);

  IDatabaseControllerLogEnable = new QCheckBox();
  IDatabaseControllerLogEnable->setCheckState(
      settings.value("IDatabaseController/LogEnable").toBool() ? Qt::Checked
                                                               : Qt::Unchecked);
  DatabaseSettingsLayout->addWidget(IDatabaseControllerLogEnable, 5, 1, 1, 1);

  // Настройки логгера
  LogSystemSettingsGroupBox = new QGroupBox("Настройки системы логгирования");
  SettingsMainSubLayout->addWidget(LogSystemSettingsGroupBox);

  LogSystemSettingsLayout = new QGridLayout();
  LogSystemSettingsGroupBox->setLayout(LogSystemSettingsLayout);

  LogSystemEnableLabel = new QLabel("Вкл/выкл системы логгирования");
  LogSystemSettingsLayout->addWidget(LogSystemEnableLabel, 0, 0, 1, 1);
  LogSystemEnableCheckBox = new QCheckBox();
  LogSystemEnableCheckBox->setCheckState(
      settings.value("Global/LogEnable").toBool() ? Qt::Checked
                                                  : Qt::Unchecked);
  LogSystemSettingsLayout->addWidget(LogSystemEnableCheckBox, 0, 1, 1, 1);
  connect(LogSystemEnableCheckBox, &QCheckBox::stateChanged, this,
          &MasterGUI::on_LogSystemEnableCheckBox_slot);

  LogSystemProxyWidget1 = new QWidget();
  LogSystemSettingsLayout->addWidget(LogSystemProxyWidget1, 3, 0, 1, 2);
  if (!LogSystemEnableCheckBox->isChecked()) {
    LogSystemProxyWidget1->hide();
  }
  LogSystemProxyWidget1Layout = new QGridLayout();
  LogSystemProxyWidget1->setLayout(LogSystemProxyWidget1Layout);

  LogSystemSavePathLabel = new QLabel("Директория для лог-файлов");
  LogSystemProxyWidget1Layout->addWidget(LogSystemSavePathLabel, 1, 0, 1, 2);
  LogSystemSavePathLineEdit =
      new QLineEdit(settings.value("LogSystem/Save/Directory").toString());
  LogSystemSavePathLineEdit->setMaxLength(300);
  LogSystemProxyWidget1Layout->addWidget(LogSystemSavePathLineEdit, 1, 1, 1, 1);
  LogSystemSavePathExplorePushButton = new QPushButton("Обзор");
  LogSystemProxyWidget1Layout->addWidget(LogSystemSavePathExplorePushButton, 1,
                                         2, 1, 1);
  connect(LogSystemSavePathExplorePushButton, &QPushButton::clicked, this,
          &MasterGUI::on_LogSystemSavePathExplorePushButton_slot);

  LogSystemListenPersoServerLabel =
      new QLabel("Получение логов с сервера персонализации");
  LogSystemProxyWidget1Layout->addWidget(LogSystemListenPersoServerLabel, 2, 0,
                                         1, 1);
  LogSystemListenPersoServerCheckBox = new QCheckBox();
  LogSystemListenPersoServerCheckBox->setCheckState(
      settings.value("LogSystem/PersoServer/Enable").toBool() ? Qt::Checked
                                                              : Qt::Unchecked);
  LogSystemProxyWidget1Layout->addWidget(LogSystemListenPersoServerCheckBox, 2,
                                         1, 1, 1);
  connect(LogSystemListenPersoServerCheckBox, &QCheckBox::stateChanged, this,
          &MasterGUI::on_LogSystemListenPersoServerCheckBox_slot);

  LogSystemProxyWidget2 = new QWidget();
  LogSystemProxyWidget1Layout->addWidget(LogSystemProxyWidget2, 3, 0, 1, 2);
  if (!LogSystemListenPersoServerCheckBox->isChecked()) {
    LogSystemProxyWidget2->hide();
  }
  LogSystemProxyWidget2Layout = new QGridLayout();
  LogSystemProxyWidget2->setLayout(LogSystemProxyWidget2Layout);

  LogSystemListenIpLabel = new QLabel("Прослушиваемый IP");
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenIpLabel, 0, 0, 1, 1);
  LogSystemListenIpLineEdit =
      new QLineEdit(settings.value("LogSystem/PersoServer/Ip").toString());
  LogSystemListenIpLineEdit->setMaxLength(300);
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenIpLineEdit, 0, 1, 1, 1);

  LogSystemListenPortLabel = new QLabel("Прослушиваемый порт");
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenPortLabel, 1, 0, 1, 1);
  LogSystemListenPortLineEdit =
      new QLineEdit(settings.value("LogSystem/PersoServer/Port").toString());
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenPortLineEdit, 1, 1, 1,
                                         1);

  // Кнопка сохранения настроек
  ApplySettingsPushButton = new QPushButton("Применить изменения");
  SettingsMainSubLayout->addWidget(ApplySettingsPushButton);

  // Сжатие по горизонтали
  SettingsVerticalSpacer1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  SettingsMainSubLayout->addItem(SettingsVerticalSpacer1);
}

void MasterGUI::displayLog(const QString& data) {
  if (LogDisplay->toPlainText().length() > 500000)
    LogDisplay->clear();

  if (LogDisplay)
    LogDisplay->appendPlainText(data);
}

void MasterGUI::clearLogDisplay() {
  LogDisplay->clear();
}

void MasterGUI::on_PanFileExplorePushButton_slot() {
  QString filePath =
      QFileDialog::getOpenFileName(nullptr, "Выбрать файл", "./", "*.csv");
  if (!filePath.isEmpty()) {
    PanFilePathLineEdit->setText(filePath);
  }
}

void MasterGUI::on_SearchTransponderByComboBox_slot(const QString& text) {
  if (text == "UCID") {
    SearchTransponderLineEdit->setMaxLength(UCID_CHAR_LENGTH);
  } else if (text == "PAN") {
    SearchTransponderLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  } else if (text == "SN") {
    SearchTransponderLineEdit->setMaxLength(
        TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  } else {
    SearchTransponderLineEdit->setMaxLength(0);
  }
}

void MasterGUI::on_RereleaseTransponderByComboBox_slot(const QString& text) {
  if (text == "PAN") {
    RereleaseTransponderLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  } else if (text == "SN") {
    RereleaseTransponderLineEdit->setMaxLength(
        TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  } else {
    RereleaseTransponderLineEdit->setMaxLength(0);
  }
}

void MasterGUI::on_LogSystemSavePathExplorePushButton_slot() {
  QString dir = QFileDialog::getExistingDirectory(
      nullptr, "Выбрать директорию", "", QFileDialog::ShowDirsOnly);
  if (!dir.isEmpty()) {
    LogSystemSavePathLineEdit->setText(dir);
  }
}

void MasterGUI::on_LogSystemEnableCheckBox_slot(int state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget1->show();
  } else {
    LogSystemProxyWidget1->hide();
  }
}

void MasterGUI::on_LogSystemListenPersoServerCheckBox_slot(int state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget2->show();
  } else {
    LogSystemProxyWidget2->hide();
  }
}
