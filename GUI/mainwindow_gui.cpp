#include "mainwindow_gui.h"
#include "General/definitions.h"

MainWindowGUI::MainWindowGUI(QWidget* parent) : AbstractGUI(parent, Master) {
  setObjectName("MainWindowGUI");
}

void MainWindowGUI::create() {
  // Создаем вкладки мастер меню
  createTabs();

  // Создаем виджеты логов
  createLog();

  // Настраиваем пропорции отображаемых элементов
  MainLayout->setStretch(0, 3);
  MainLayout->setStretch(1, 2);
}

void MainWindowGUI::update() {
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

void MainWindowGUI::displayLog(const QString& data) {
  if (LogDisplay->toPlainText().length() > 500000)
    LogDisplay->clear();

  if (LogDisplay)
    LogDisplay->appendPlainText(data);
}

void MainWindowGUI::clearLogDisplay() {
  LogDisplay->clear();
}

void MainWindowGUI::createTabs() {
  Tabs = new QTabWidget();
  MainLayout->addWidget(Tabs);

  // Задаем стартовую страницу
  Tabs->setCurrentIndex(0);

  createDatabaseTab();
  createOrderTab();
  createProductionLineTab();
  createServerTab();
  createTransponderTab();
  createIssuerTab();
  createStickerTab();
  createSettingsTab();
}

void MainWindowGUI::createDatabaseTab() {
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

void MainWindowGUI::createOrderTab() {
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

void MainWindowGUI::createProductionLineTab() {
  ProductionLinesTab = new QWidget();
  Tabs->addTab(ProductionLinesTab, "Линии производства");

  ProductionLinesTabMainLayout = new QHBoxLayout();
  ProductionLinesTab->setLayout(ProductionLinesTabMainLayout);

  // Панель управления
  ProductionLinesControlPanel = new QGroupBox("Панель управления");
  ProductionLinesTabMainLayout->addWidget(ProductionLinesControlPanel);

  ProductionLinesControlPanelLayout = new QVBoxLayout();
  ProductionLinesControlPanel->setLayout(ProductionLinesControlPanelLayout);

  CreateNewProductionLinePushButton =
      new QPushButton("Создать производственную линию");
  ProductionLinesControlPanelLayout->addWidget(
      CreateNewProductionLinePushButton);
  StartProductionLinePushButton =
      new QPushButton("Запустить производственную линию");
  ProductionLinesControlPanelLayout->addWidget(StartProductionLinePushButton);
  StopProductionLinePushButton =
      new QPushButton("Остановить производственную линию");
  ProductionLinesControlPanelLayout->addWidget(StopProductionLinePushButton);
  ShutdownAllProductionLinesPushButton =
      new QPushButton("Остановить все производственные линии");
  ProductionLinesControlPanelLayout->addWidget(
      ShutdownAllProductionLinesPushButton);
  EditProductionLinesPushButton =
      new QPushButton("Редактировать производственную линию");
  ProductionLinesControlPanelLayout->addWidget(EditProductionLinesPushButton);

  ProductionLinesControlPanelVS2 =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ProductionLinesControlPanelLayout->addItem(ProductionLinesControlPanelVS2);

  UpdateProductionLineViewPushButton = new QPushButton("Обновить таблицу");
  ProductionLinesControlPanelLayout->addWidget(
      UpdateProductionLineViewPushButton);

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

void MainWindowGUI::createServerTab() {
  ServerTab = new QWidget();
  Tabs->addTab(ServerTab, "Cервер");

  ServerTabMainLayout = new QHBoxLayout();
  ServerTab->setLayout(ServerTabMainLayout);

  // Панель управления
  ServerTabControlPanel = new QGroupBox("Панель управления");
  ServerTabMainLayout->addWidget(ServerTabControlPanel);

  ServerTabControlPanelLayout = new QVBoxLayout();
  ServerTabControlPanel->setLayout(ServerTabControlPanelLayout);

  LoginLayout2 = new QHBoxLayout();
  ServerTabControlPanelLayout->addLayout(LoginLayout2);
  LoginLabel2 = new QLabel("Логин: ");
  LoginLayout2->addWidget(LoginLabel2);
  LoginLineEdit2 = new QLineEdit();
  LoginLineEdit2->setText("1");
  LoginLineEdit2->setMaxLength(PRODUCTION_LINE_LOGIN_MAX_LENGTH);
  LoginLayout2->addWidget(LoginLineEdit2);

  PasswordLayout2 = new QHBoxLayout();
  ServerTabControlPanelLayout->addLayout(PasswordLayout2);
  PasswordLabel2 = new QLabel("Пароль: ");
  PasswordLayout2->addWidget(PasswordLabel2);
  PasswordLineEdit2 = new QLineEdit();
  PasswordLineEdit2->setText("1");
  PasswordLineEdit2->setMaxLength(PRODUCTION_LINE_PASSWORD_MAX_LENGTH);
  PasswordLayout2->addWidget(PasswordLineEdit2);

  UcidLayout = new QHBoxLayout();
  ServerTabControlPanelLayout->addLayout(UcidLayout);
  UcidLabel = new QLabel("UCID: ");
  UcidLayout->addWidget(UcidLabel);
  UcidLineEdit = new QLineEdit();
  UcidLineEdit->setMaxLength(UCID_CHAR_LENGTH);
  UcidLineEdit->setText("11111111111111111111111111111111");
  UcidLayout->addWidget(UcidLineEdit);

  ReleaseTransponderPushButton = new QPushButton("Выпустить");
  ServerTabControlPanelLayout->addWidget(ReleaseTransponderPushButton);
  ConfirmTransponderPushButton = new QPushButton("Подтвердить");
  ServerTabControlPanelLayout->addWidget(ConfirmTransponderPushButton);

  RereleaseKeyLayout = new QHBoxLayout();
  ServerTabControlPanelLayout->addLayout(RereleaseKeyLayout);
  RereleaseKeyComboBox = new QComboBox();
  RereleaseKeyComboBox->addItem("PAN");
  RereleaseKeyComboBox->addItem("SN");
  RereleaseKeyComboBox->setCurrentIndex(0);
  RereleaseKeyLayout->addWidget(RereleaseKeyComboBox);
  connect(RereleaseKeyComboBox, &QComboBox::currentTextChanged, this,
          &MainWindowGUI::on_RereleaseKeyComboBox_slot);
  RereleaseKeyLineEdit = new QLineEdit();
  RereleaseKeyLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  RereleaseKeyLayout->addWidget(RereleaseKeyLineEdit);

  RereleaseTransponderPushButton = new QPushButton("Перевыпустить");
  ServerTabControlPanelLayout->addWidget(RereleaseTransponderPushButton);
  ConfirmRereleaseTransponderPushButton =
      new QPushButton("Подтвердить перевыпуск");
  ServerTabControlPanelLayout->addWidget(ConfirmRereleaseTransponderPushButton);
  ProductionLineRollbackPushButton =
      new QPushButton("Откат производственной линии");
  ServerTabControlPanelLayout->addWidget(ProductionLineRollbackPushButton);

  ServerTabControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  ServerTabControlPanelLayout->addItem(ServerTabControlPanelVS);

  PrintBoxStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для бокса");
  ServerTabControlPanelLayout->addWidget(PrintBoxStickerOnServerPushButton);
  PrintLastBoxStickerOnServerPushButton =
      new QPushButton("Повторить последний стикер для бокса");
  ServerTabControlPanelLayout->addWidget(PrintLastBoxStickerOnServerPushButton);
  PrintPalletStickerOnServerPushButton =
      new QPushButton("Распечатать стикер для паллет");
  ServerTabControlPanelLayout->addWidget(PrintPalletStickerOnServerPushButton);
  PrintLastPalletStickerOnServerPushButton =
      new QPushButton("Повторить последний стикер для паллеты");
  ServerTabControlPanelLayout->addWidget(
      PrintLastPalletStickerOnServerPushButton);

  // Панель отображения
  TransponderDisplayPanel = new QGroupBox("Данные транспондера");
  ServerTabMainLayout->addWidget(TransponderDisplayPanel);

  TransponderDisplayLayout = new QVBoxLayout();
  TransponderDisplayPanel->setLayout(TransponderDisplayLayout);

  TransponderDataTableView = new QTableView();
  TransponderDisplayLayout->addWidget(TransponderDataTableView);

  AssembledFirmwareView = new QPlainTextEdit();
  TransponderDisplayLayout->addWidget(AssembledFirmwareView);

  // Настройка пропорции между объектами на основном макете
  ServerTabMainLayout->setStretch(0, 1);
  ServerTabMainLayout->setStretch(1, 3);
}

void MainWindowGUI::createTransponderTab() {
  TransponderTab = new QWidget();
  Tabs->addTab(TransponderTab, "Транспондеры");

  // Основной макет
  TransponderTabMainLayout = new QHBoxLayout();
  TransponderTab->setLayout(TransponderTabMainLayout);

  // Панель управления
  TransponderControlPanel = new QGroupBox(QString("Панель управления"));
  TransponderControlPanel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  TransponderTabMainLayout->addWidget(TransponderControlPanel);

  TransponderControlPanelLayout = new QVBoxLayout();
  TransponderControlPanel->setLayout(TransponderControlPanelLayout);

  TransponderControlPanelSublayout = new QHBoxLayout();
  TransponderControlPanelLayout->addLayout(TransponderControlPanelSublayout);

  ChoiceAnyIdComboBox = new QComboBox();
  ChoiceAnyIdComboBox->addItem("ID транспондера");
  ChoiceAnyIdComboBox->addItem("ID бокса");
  ChoiceAnyIdComboBox->addItem("ID паллеты");
  ChoiceAnyIdComboBox->addItem("ID заказа");
  ChoiceAnyIdComboBox->setCurrentIndex(0);
  TransponderControlPanelSublayout->addWidget(ChoiceAnyIdComboBox);
  AnyIdLineEdit = new QLineEdit();
  TransponderControlPanelSublayout->addWidget(AnyIdLineEdit);

  TransponderManualReleasePushButton = new QPushButton("Принудительный выпуск");
  TransponderControlPanelLayout->addWidget(TransponderManualReleasePushButton);
  TransponderManualRefundPushButton = new QPushButton("Произвести возврат");
  TransponderControlPanelLayout->addWidget(TransponderManualRefundPushButton);
  TransponderControlPanelLayoutVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  TransponderControlPanelLayout->addItem(TransponderControlPanelLayoutVS);

  PalletShipmentPushButton = new QPushButton("Генерация реестра для отгрузки");
  TransponderControlPanelLayout->addWidget(PalletShipmentPushButton);

  // Отображение буфера считанных данных из БД
  TransponderViewGroup = new QGroupBox(QString("Транспондеры"));
  TransponderViewGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  TransponderTabMainLayout->addWidget(TransponderViewGroup);

  TransponderViewGroupLayout = new QVBoxLayout();
  TransponderViewGroup->setLayout(TransponderViewGroupLayout);

  TransponderTableView = new QTableView();
  TransponderViewGroupLayout->addWidget(TransponderTableView);

  // Настройка пропорции между объектами на макете
  TransponderTabMainLayout->setStretch(0, 1);
  TransponderTabMainLayout->setStretch(1, 3);
}

void MainWindowGUI::createIssuerTab() {
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

void MainWindowGUI::createStickerTab() {
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

void MainWindowGUI::createSettingsTab() {
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

  DatabaseIpLineEdit = new QLineEdit(
      settings.value("postgre_sql_database/server_ip").toString());
  DatabaseSettingsLayout->addWidget(DatabaseIpLineEdit, 0, 1, 1, 1);

  DatabasePortLabel = new QLabel("Порт ");
  DatabaseSettingsLayout->addWidget(DatabasePortLabel, 1, 0, 1, 1);

  DatabasePortLineEdit = new QLineEdit(
      settings.value("postgre_sql_database/server_port").toString());
  DatabaseSettingsLayout->addWidget(DatabasePortLineEdit, 1, 1, 1, 1);

  DatabaseNameLabel = new QLabel("Название базы данных ");
  DatabaseSettingsLayout->addWidget(DatabaseNameLabel, 2, 0, 1, 1);

  DatabaseNameLineEdit = new QLineEdit(
      settings.value("postgre_sql_database/database_name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseNameLineEdit, 2, 1, 1, 1);

  DatabaseUserNameLabel = new QLabel("Имя пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLabel, 3, 0, 1, 1);

  DatabaseUserNameLineEdit = new QLineEdit(
      settings.value("postgre_sql_database/user_name").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserNameLineEdit, 3, 1, 1, 1);

  DatabaseUserPasswordLabel = new QLabel("Пароль пользователя ");
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLabel, 4, 0, 1, 1);

  DatabaseUserPasswordLineEdit = new QLineEdit(
      settings.value("postgre_sql_database/user_password").toString());
  DatabaseSettingsLayout->addWidget(DatabaseUserPasswordLineEdit, 4, 1, 1, 1);

  IDatabaseControllerLogEnableLabel = new QLabel("Логирование ");
  DatabaseSettingsLayout->addWidget(IDatabaseControllerLogEnableLabel, 5, 0, 1,
                                    1);

  IDatabaseControllerLogEnable = new QCheckBox();
  IDatabaseControllerLogEnable->setCheckState(
      settings.value("postgre_sql_database/log_enable").toBool()
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
          &MainWindowGUI::on_LogSystemEnableCheckBox_slot);

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
          &MainWindowGUI::on_LogSystemListenPersoServerCheckBox_slot);

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
          &MainWindowGUI::on_LogSystemFileEnableCheckBox_slot);

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
          &MainWindowGUI::on_StickerPrinterLibPathPushButton_slot);

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

void MainWindowGUI::createLog() {
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

void MainWindowGUI::on_RereleaseKeyComboBox_slot(const QString& text) {
  if (text == "PAN") {
    RereleaseKeyLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  } else if (text == "SN") {
    RereleaseKeyLineEdit->setMaxLength(TRANSPONDER_SERIAL_NUMBER_CHAR_LENGTH);
  } else {
    RereleaseKeyLineEdit->setMaxLength(0);
  }
}

void MainWindowGUI::on_LogSystemEnableCheckBox_slot(int state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget1->show();
  } else {
    LogSystemProxyWidget1->hide();
  }
}

void MainWindowGUI::on_LogSystemListenPersoServerCheckBox_slot(int state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget2->show();
  } else {
    LogSystemProxyWidget2->hide();
  }
}

void MainWindowGUI::on_LogSystemFileEnableCheckBox_slot(int32_t state) {
  if (state == Qt::Checked) {
    LogSystemProxyWidget3->show();
  } else {
    LogSystemProxyWidget3->hide();
  }
}

void MainWindowGUI::on_StickerPrinterLibPathPushButton_slot() {
  QString filePath =
      QFileDialog::getOpenFileName(this, "Выберите файл", "", "*.dll");
  StickerPrinterLibPathLineEdit->setText(filePath);
}
