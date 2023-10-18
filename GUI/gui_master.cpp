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
  MainLayout->setStretch(0, 3);
  MainLayout->setStretch(1, 2);
}

void MasterGUI::update() {
  DatabaseRandomModelView->resizeColumnsToContents();
  DatabaseRandomModelView->update();

  OrderTableView->resizeColumnsToContents();
  OrderTableView->update();

  ProductionLineTableView->resizeColumnsToContents();
  ProductionLineTableView->update();

  TransponderDataTableView->resizeColumnsToContents();
  TransponderDataTableView->update();

  IssuerTableView->resizeColumnsToContents();
  IssuerTableView->update();

  IssuerTableView->resizeColumnsToContents();
  IssuerTableView->update();

  StickerDataTableView->resizeColumnsToContents();
  StickerDataTableView->update();
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

void MasterGUI::createTabs() {
  Tabs = new QTabWidget();
  MainLayout->addWidget(Tabs);

  // Задаем стартовую страницу
  Tabs->setCurrentIndex(0);

  createDatabaseTab();
  createOrderTab();
  createProductionLineTab();
  createServerTab();
  createIssuerTab();
  createStickerTab();
  createSettingsTab();
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
  LoginLineEdit1->setMaxLength(PRODUCTION_LINE_LOGIN_MAX_LENGTH);
  LoginLayout1->addWidget(LoginLineEdit1);
  PasswordLayout1 = new QHBoxLayout();
  ProductionLinesControlPanelLayout->addLayout(PasswordLayout1);
  PasswordLabel1 = new QLabel("Пароль: ");
  PasswordLayout1->addWidget(PasswordLabel1);
  PasswordLineEdit1 = new QLineEdit();
  PasswordLineEdit1->setMaxLength(PRODUCTION_LINE_PASSWORD_MAX_LENGTH);
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
  BoxIdLineEdit1 = new QLineEdit();
  BoxIdLayout->addWidget(BoxIdLineEdit1);
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

void MasterGUI::createServerTab() {
  TransponderTab = new QWidget();
  Tabs->addTab(TransponderTab, "Cервер");

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
  LoginLineEdit2->setMaxLength(PRODUCTION_LINE_LOGIN_MAX_LENGTH);
  LoginLayout2->addWidget(LoginLineEdit2);

  PasswordLayout2 = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(PasswordLayout2);
  PasswordLabel2 = new QLabel("Пароль: ");
  PasswordLayout2->addWidget(PasswordLabel2);
  PasswordLineEdit2 = new QLineEdit();
  PasswordLineEdit2->setText("1");
  PasswordLineEdit2->setMaxLength(PRODUCTION_LINE_PASSWORD_MAX_LENGTH);
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

  RereleaseKeyLayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(RereleaseKeyLayout);
  RereleaseKeyComboBox = new QComboBox();
  RereleaseKeyComboBox->addItem("PAN");
  RereleaseKeyComboBox->addItem("SN");
  RereleaseKeyComboBox->setCurrentIndex(0);
  RereleaseKeyLayout->addWidget(RereleaseKeyComboBox);
  connect(RereleaseKeyComboBox, &QComboBox::currentTextChanged, this,
          &MasterGUI::on_RereleaseKeyComboBox_slot);
  RereleaseKeyLineEdit = new QLineEdit();
  RereleaseKeyLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  RereleaseKeyLayout->addWidget(RereleaseKeyLineEdit);

  RereleaseTransponderPushButton = new QPushButton("Перевыпустить");
  TransponderControlPanelLayout->addWidget(RereleaseTransponderPushButton);
  ConfirmRereleaseTransponderPushButton =
      new QPushButton("Подтвердить перевыпуск");
  TransponderControlPanelLayout->addWidget(
      ConfirmRereleaseTransponderPushButton);

  TransponderControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  TransponderControlPanelLayout->addItem(TransponderControlPanelVS);

  PrintBoxStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для бокса");
  TransponderControlPanelLayout->addWidget(PrintBoxStickerOnServerPushButton);
  PrintLastBoxStickerOnServerPushButton =
      new QPushButton("Повторить последний стикера для бокса");
  TransponderControlPanelLayout->addWidget(
      PrintLastBoxStickerOnServerPushButton);
  PrintPalletStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для паллет");
  TransponderControlPanelLayout->addWidget(
      PrintPalletStickerOnServerPushButton);
  PrintLastPalletStickerOnServerPushButton =
      new QPushButton("Повторить последний стикера для паллеты");
  TransponderControlPanelLayout->addWidget(
      PrintLastPalletStickerOnServerPushButton);

  // Панель отображения
  TransponderDisplayPanel = new QGroupBox("Данные транспондера");
  TransponderTabMainLayout->addWidget(TransponderDisplayPanel);

  TransponderDisplayLayout = new QVBoxLayout();
  TransponderDisplayPanel->setLayout(TransponderDisplayLayout);

  TransponderDataTableView = new QTableView();
  TransponderDisplayLayout->addWidget(TransponderDataTableView);

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
  IssuerTableChoice->addItem("Эмитенты");
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

void MasterGUI::createStickerTab() {
  StickerTab = new QWidget();
  Tabs->addTab(StickerTab, "Стикеры");

  // Основной макет
  StickerMainLayout = new QHBoxLayout();
  StickerTab->setLayout(StickerMainLayout);

  // Панель управления
  StickerControlPanel = new QGroupBox(QString("Панель управления"));
  StickerControlPanel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  StickerMainLayout->addWidget(StickerControlPanel);

  StickerControlPanelLayout = new QVBoxLayout();
  StickerControlPanel->setLayout(StickerControlPanelLayout);

  TransponderIdLineEdit = new QLineEdit();
  StickerControlPanelLayout->addWidget(TransponderIdLineEdit);
  PrintTransponderStickerPushButton =
      new QPushButton("Распечатать стикер для транспондера");
  StickerControlPanelLayout->addWidget(PrintTransponderStickerPushButton);
  BoxIdLineEdit2 = new QLineEdit();
  StickerControlPanelLayout->addWidget(BoxIdLineEdit2);
  PrintBoxStickerPushButton = new QPushButton("Распечатать стикер для бокса");
  StickerControlPanelLayout->addWidget(PrintBoxStickerPushButton);
  PalletIdLineEdit = new QLineEdit();
  StickerControlPanelLayout->addWidget(PalletIdLineEdit);
  PrintPalletStickerPushButton =
      new QPushButton("Распечатать стикер для паллеты");
  StickerControlPanelLayout->addWidget(PrintPalletStickerPushButton);

  StickerControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  StickerControlPanelLayout->addItem(StickerControlPanelVS);

  ExecStickerPrinterCommandScriptPushButton =
      new QPushButton("Выполнить скрипт");
  StickerControlPanelLayout->addWidget(
      ExecStickerPrinterCommandScriptPushButton);

  // Отображение информации о стикере
  StickerDataViewGroup = new QGroupBox(QString("Данные стикера"));
  StickerDataViewGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  StickerMainLayout->addWidget(StickerDataViewGroup);

  StickerDataViewLayout = new QVBoxLayout();
  StickerDataViewGroup->setLayout(StickerDataViewLayout);

  StickerDataTableView = new QTableView();
  StickerDataViewLayout->addWidget(StickerDataTableView);
  StickerPrinterCommandScriptInput = new QPlainTextEdit();
  StickerDataViewLayout->addWidget(StickerPrinterCommandScriptInput);

  // Настройка пропорций
  StickerMainLayout->setStretch(0, 1);
  StickerMainLayout->setStretch(1, 2);
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

  // Настройки базы данных
  DatabaseSettingsGroupBox = new QGroupBox(QString("Настройки базы данных"));
  SettingsMainSubLayout->addWidget(DatabaseSettingsGroupBox);

  DatabaseSettingsLayout = new QGridLayout();
  DatabaseSettingsGroupBox->setLayout(DatabaseSettingsLayout);

  DatabaseIpLabel = new QLabel("IP-адрес");
  DatabaseSettingsLayout->addWidget(DatabaseIpLabel, 0, 0, 1, 1);

  DatabaseIpLineEdit =
      new QLineEdit(settings.value("postgres_controller/server_ip").toString());
  DatabaseSettingsLayout->addWidget(DatabaseIpLineEdit, 0, 1, 1, 1);

  DatabasePortLabel = new QLabel("Порт ");
  DatabaseSettingsLayout->addWidget(DatabasePortLabel, 1, 0, 1, 1);

  DatabasePortLineEdit = new QLineEdit(
      settings.value("postgres_controller/server_port").toString());
  DatabaseSettingsLayout->addWidget(DatabasePortLineEdit, 1, 1, 1, 1);

  DatabaseNameLabel = new QLabel("Название базы данных ");
  DatabaseSettingsLayout->addWidget(DatabaseNameLabel, 2, 0, 1, 1);

  DatabaseNameLineEdit = new QLineEdit(
      settings.value("postgres_controller/database_name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseNameLineEdit, 2, 1, 1, 1);

  DatabaseUserNameLabel = new QLabel("Имя пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLabel, 3, 0, 1, 1);

  DatabaseUserNameLineEdit =
      new QLineEdit(settings.value("postgres_controller/user_name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLineEdit, 3, 1, 1, 1);

  DatabaseUserPasswordLabel = new QLabel("Пароль пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLabel, 4, 0, 1, 1);

  DatabaseUserPasswordLineEdit = new QLineEdit(
      settings.value("postgres_controller/user_password").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLineEdit, 4, 1, 1, 1);

  IDatabaseControllerLogEnableLabel = new QLabel("Логирование ");
  DatabaseSettingsLayout->addWidget(IDatabaseControllerLogEnableLabel, 5, 0, 1,
                                    1);

  IDatabaseControllerLogEnable = new QCheckBox();
  IDatabaseControllerLogEnable->setCheckState(
      settings.value("postgres_controller/log_enable").toBool()
          ? Qt::Checked
          : Qt::Unchecked);
  DatabaseSettingsLayout->addWidget(IDatabaseControllerLogEnable, 5, 1, 1, 1);

  // Настройки клиента
  PersoClientSettingsGroupBox = new QGroupBox(QString("Сетевые настройки"));
  SettingsMainSubLayout->addWidget(PersoClientSettingsGroupBox);

  PersoClientSettingsMainLayout = new QGridLayout();
  PersoClientSettingsGroupBox->setLayout(PersoClientSettingsMainLayout);

  PersoClientServerIdLabel =
      new QLabel("IP адрес или URL сервера персонализации");
  PersoClientSettingsMainLayout->addWidget(PersoClientServerIdLabel, 0, 0, 1,
                                           1);
  PersoClientServerIpLineEdit =
      new QLineEdit(settings.value("perso_client/server_ip").toString());
  PersoClientSettingsMainLayout->addWidget(PersoClientServerIpLineEdit, 0, 1, 1,
                                           1);
  PersoClientServerPortLabel = new QLabel("Порт сервера персонализации");
  PersoClientSettingsMainLayout->addWidget(PersoClientServerPortLabel, 1, 0, 1,
                                           1);
  PersoClientServerPortLineEdit =
      new QLineEdit(settings.value("perso_client/server_port").toString());
  PersoClientSettingsMainLayout->addWidget(PersoClientServerPortLineEdit, 1, 1,
                                           1, 1);

  // Настройки логгера
  LogSystemSettingsGroupBox = new QGroupBox("Настройки системы логгирования");
  SettingsMainSubLayout->addWidget(LogSystemSettingsGroupBox);

  LogSystemSettingsLayout = new QGridLayout();
  LogSystemSettingsGroupBox->setLayout(LogSystemSettingsLayout);

  LogSystemGlobalEnableLabel = new QLabel("Глобальное включение");
  LogSystemSettingsLayout->addWidget(LogSystemGlobalEnableLabel, 0, 0, 1, 1);
  LogSystemGlobalEnableCheckBox = new QCheckBox();
  LogSystemGlobalEnableCheckBox->setCheckState(
      settings.value("log_system/global_enable").toBool() ? Qt::Checked
                                                          : Qt::Unchecked);
  LogSystemSettingsLayout->addWidget(LogSystemGlobalEnableCheckBox, 0, 1, 1, 1);
  connect(LogSystemGlobalEnableCheckBox, &QCheckBox::stateChanged, this,
          &MasterGUI::on_LogSystemEnableCheckBox_slot);

  LogSystemExtendedEnableLabel = new QLabel("Расширенное логгирование");
  LogSystemSettingsLayout->addWidget(LogSystemExtendedEnableLabel, 1, 0, 1, 1);
  LogSystemExtendedEnableCheckBox = new QCheckBox();
  LogSystemExtendedEnableCheckBox->setCheckState(
      settings.value("log_system/extended_enable").toBool() ? Qt::Checked
                                                            : Qt::Unchecked);
  LogSystemSettingsLayout->addWidget(LogSystemExtendedEnableCheckBox, 1, 1, 1,
                                     1);

  LogSystemProxyWidget1 = new QWidget();
  LogSystemSettingsLayout->addWidget(LogSystemProxyWidget1, 2, 0, 1, 2);
  if (!LogSystemGlobalEnableCheckBox->isChecked()) {
    LogSystemProxyWidget1->hide();
  }
  LogSystemProxyWidget1Layout = new QGridLayout();
  LogSystemProxyWidget1->setLayout(LogSystemProxyWidget1Layout);

  LogSystemDisplayEnableLabel = new QLabel("Вывод на дисплей вкл/выкл");
  LogSystemProxyWidget1Layout->addWidget(LogSystemDisplayEnableLabel, 0, 0, 1,
                                         1);
  LogSystemDisplayEnableCheckBox = new QCheckBox();
  LogSystemDisplayEnableCheckBox->setCheckState(
      settings.value("log_system/display_log_enable").toBool() ? Qt::Checked
                                                               : Qt::Unchecked);
  LogSystemProxyWidget1Layout->addWidget(LogSystemDisplayEnableCheckBox, 0, 1,
                                         1, 1);

  LogSystemListenPersoServerLabel =
      new QLabel("Получение логов с сервера персонализации");
  LogSystemProxyWidget1Layout->addWidget(LogSystemListenPersoServerLabel, 1, 0,
                                         1, 1);
  LogSystemListenPersoServerCheckBox = new QCheckBox();
  LogSystemListenPersoServerCheckBox->setCheckState(
      settings.value("log_system/udp_listen_enable").toBool() ? Qt::Checked
                                                              : Qt::Unchecked);
  LogSystemProxyWidget1Layout->addWidget(LogSystemListenPersoServerCheckBox, 1,
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
      new QLineEdit(settings.value("log_system/udp_listen_ip").toString());
  LogSystemListenIpLineEdit->setMaxLength(300);
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenIpLineEdit, 0, 1, 1, 1);

  LogSystemListenPortLabel = new QLabel("Прослушиваемый порт");
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenPortLabel, 1, 0, 1, 1);
  LogSystemListenPortLineEdit =
      new QLineEdit(settings.value("log_system/udp_listen_port").toString());
  LogSystemProxyWidget2Layout->addWidget(LogSystemListenPortLineEdit, 1, 1, 1,
                                         1);

  LogSystemFileEnableLabel = new QLabel("Логгирование в файл");
  LogSystemProxyWidget1Layout->addWidget(LogSystemFileEnableLabel, 4, 0, 1, 1);
  LogSystemFileEnableCheckBox = new QCheckBox();
  LogSystemFileEnableCheckBox->setCheckState(
      settings.value("log_system/file_log_enable").toBool() ? Qt::Checked
                                                            : Qt::Unchecked);
  LogSystemProxyWidget1Layout->addWidget(LogSystemFileEnableCheckBox, 4, 1, 1,
                                         1);
  connect(LogSystemFileEnableCheckBox, &QCheckBox::stateChanged, this,
          &MasterGUI::on_LogSystemFileEnableCheckBox_slot);

  LogSystemProxyWidget3 = new QWidget();
  LogSystemProxyWidget1Layout->addWidget(LogSystemProxyWidget3, 5, 0, 1, 2);
  if (!LogSystemFileEnableCheckBox->isChecked()) {
    LogSystemProxyWidget3->hide();
  }
  LogSystemProxyWidget3Layout = new QGridLayout();
  LogSystemProxyWidget3->setLayout(LogSystemProxyWidget3Layout);

  LogSystemFileMaxNumberLabel =
      new QLabel("Максимальное количество лог-файлов");
  LogSystemProxyWidget3Layout->addWidget(LogSystemFileMaxNumberLabel, 0, 0, 1,
                                         1);
  LogSystemFileMaxNumberLineEdit = new QLineEdit(
      settings.value("log_system/log_file_max_number").toString());
  LogSystemFileMaxNumberLineEdit->setMaxLength(10);
  LogSystemProxyWidget3Layout->addWidget(LogSystemFileMaxNumberLineEdit, 0, 1,
                                         1, 1);

  // Настройки принтера стикеров
  StickerPrinterSettingsGroupBox = new QGroupBox(QString("Стикер-принтер"));
  SettingsMainSubLayout->addWidget(StickerPrinterSettingsGroupBox);

  StickerPrinterSettingsMainLayout = new QGridLayout();
  StickerPrinterSettingsGroupBox->setLayout(StickerPrinterSettingsMainLayout);

  StickerPrinterLibPathLabel = new QLabel("Путь к библиотеке");
  StickerPrinterSettingsMainLayout->addWidget(StickerPrinterLibPathLabel, 0, 0,
                                              1, 1);
  StickerPrinterLibPathLineEdit =
      new QLineEdit(settings.value("sticker_printer/library_path").toString());
  StickerPrinterSettingsMainLayout->addWidget(StickerPrinterLibPathLineEdit, 0,
                                              1, 1, 1);
  StickerPrinterLibPathPushButton = new QPushButton("Обзор");
  StickerPrinterSettingsMainLayout->addWidget(StickerPrinterLibPathPushButton,
                                              0, 2, 1, 1);
  connect(StickerPrinterLibPathPushButton, &QPushButton::clicked, this,
          &MasterGUI::on_StickerPrinterLibPathPushButton_slot);

  StickerPrinterNameLabel = new QLabel("Название");
  StickerPrinterSettingsMainLayout->addWidget(StickerPrinterNameLabel, 1, 0, 1,
                                              1);
  StickerPrinterNameLineEdit =
      new QLineEdit(settings.value("sticker_printer/name").toString());
  StickerPrinterSettingsMainLayout->addWidget(StickerPrinterNameLineEdit, 1, 1,
                                              1, 2);

  // Кнопка сохранения настроек
  ApplySettingsPushButton = new QPushButton("Применить изменения");
  SettingsMainSubLayout->addWidget(ApplySettingsPushButton);

  // Сжатие по горизонтали
  SettingsVerticalSpacer1 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  SettingsMainSubLayout->addItem(SettingsVerticalSpacer1);
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
void MasterGUI::on_PanFileExplorePushButton_slot() {
  QString filePath =
      QFileDialog::getOpenFileName(nullptr, "Выбрать файл", "./", "*.csv");
  if (!filePath.isEmpty()) {
    PanFilePathLineEdit->setText(filePath);
  }
}

void MasterGUI::on_RereleaseKeyComboBox_slot(const QString& text) {
  if (text == "PAN") {
    RereleaseKeyLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  } else if (text == "SN") {
    RereleaseKeyLineEdit->setMaxLength(TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  } else {
    RereleaseKeyLineEdit->setMaxLength(0);
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

void MasterGUI::on_LogSystemFileEnableCheckBox_slot(int32_t state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget3->show();
  } else {
    LogSystemProxyWidget3->hide();
  }
}

void MasterGUI::on_StickerPrinterLibPathPushButton_slot() {
  QString filePath =
      QFileDialog::getOpenFileName(this, "Выберите файл", "", "*.dll");
  StickerPrinterLibPathLineEdit->setText(filePath);
}
