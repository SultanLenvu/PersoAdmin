#include "main_window_kernel.h"

MainWindowKernel::MainWindowKernel(QWidget* parent) : QMainWindow(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::desktop()->screenGeometry();

  // Загружаем пользовательские настройки
  loadSettings();

  // Графический интерфейс пока не создан
  CurrentGUI = nullptr;
  Manager = nullptr;
  Logger = nullptr;
  Interactor = nullptr;

  // Создаем систему логгирования
  createLogger();

  // Создаем графический интерфейс окна авторизации
  createAuthorazationGui();

  // Cистема взаимодействия с пользователем
  createInterructor();

  // Управляющий модуль
  createManager();

  // Создаем модели для представлений
  createModels();

  // Создаем таблицу соответствий
  createMatchingTable();
}

MainWindowKernel::~MainWindowKernel() {}

void MainWindowKernel::on_RequestAuthorizationGuiAct_slot() {
  createAuthorazationGui();
}

void MainWindowKernel::on_AuthorizePushButton_slot() {
  if (!checkAuthorizationData()) {
    Interactor->generateError("Неверный логин или пароль. ");
    return;
  }

  createMasterGui();

  Manager->performDatabaseConnecting();
}

void MainWindowKernel::on_ShowDatabaseTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  Manager->showDatabaseTable(gui->DatabaseTableChoice->currentText(),
                             RandomModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_ClearDatabaseTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  Manager->clearDatabaseTable(gui->DatabaseTableChoice->currentText(),
                              RandomModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_TransmitCustomRequestPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  Manager->performCustomRequest(gui->CustomRequestLineEdit->text(),
                                RandomModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_CreateNewOrderPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);

  Logger->clear();

  if (!checkNewOrderInput()) {
    Interactor->generateError("Некорректный ввод параметров нового заказа. ");
    return;
  }

  QMap<QString, QString> orderParameters;
  orderParameters.insert("issuer_name", gui->IssuerNameComboBox->currentText());
  orderParameters.insert("transponder_quantity",
                         gui->TransponderQuantityLineEdit->text());
  orderParameters.insert("box_capacity", gui->BoxCapacityLineEdit->text());
  orderParameters.insert("pallet_capacity",
                         gui->PalletCapacityLineEdit->text());
  orderParameters.insert(
      "full_personalization",
      gui->FullPersonalizationCheckBox->checkState() == Qt::Checked ? "true"
                                                                    : "false");
  orderParameters.insert("pan_file_path", gui->PanFilePathLineEdit->text());
  orderParameters.insert("transponder_model",
                         gui->TransponderModelLineEdit->text());
  orderParameters.insert("accr_reference", gui->AccrReferenceLineEdit->text());
  orderParameters.insert("equipment_class",
                         gui->EquipmentClassLineEdit->text());
  orderParameters.insert("manufacturer_id",
                         gui->ManufacturerIdLineEdit->text());

  Manager->createNewOrder(&orderParameters, OrderModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_StartOrderAssemblingPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  if (gui->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  Manager->startOrderAssemblingManually(gui->OrderIdLineEdit1->text(),
                                        OrderModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_StopOrderAssemblingPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  if (gui->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  Manager->stopOrderAssemblingManually(gui->OrderIdLineEdit1->text(),
                                       OrderModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_UpdateOrderViewPushButton_slot() {
  Logger->clear();

  Manager->showDatabaseTable("orders", OrderModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_DeleteLastOrderPushButton_slot() {
  Logger->clear();

  Manager->deleteLastOrder(OrderModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_CreateNewProductionLinePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  if (!checkNewProductionLineInput()) {
    Interactor->generateError(
        "Некорректный ввод параметров нового производственной линии. ");
    return;
  }

  QMap<QString, QString> productionLineParameters;
  productionLineParameters.insert("login", gui->LoginLineEdit1->text());
  productionLineParameters.insert("password", gui->PasswordLineEdit1->text());
  Manager->createNewProductionLine(&productionLineParameters,
                                   ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_AllocateInactiveProductionLinesPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  if (gui->OrderIdLineEdit2->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  Manager->allocateInactiveProductionLinesManually(
      gui->OrderIdLineEdit2->text(), ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_LinkProductionLinePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  Logger->clear();

  if ((!checkNewProductionLineInput()) ||
      (gui->BoxIdLineEdit->text().toInt() == 0)) {
    Interactor->generateError(
        "Некорректный ввод параметров производственной линии. ");
    return;
  }

  QMap<QString, QString> linkParameters;
  linkParameters.insert("login", gui->LoginLineEdit1->text());
  linkParameters.insert("password", gui->PasswordLineEdit1->text());
  linkParameters.insert("box_id", gui->BoxIdLineEdit->text());

  Manager->linkProductionLineWithBoxManually(&linkParameters,
                                             ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_DeactivateAllProductionLinesPushButton_slot() {
  Logger->clear();

  Manager->shutdownAllProductionLinesManually(ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_UpdateProductionLineViewPushButton_slot() {
  Logger->clear();

  Manager->showDatabaseTable("production_lines", ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_DeleteLastProductionLinePushButton_slot() {
  Logger->clear();

  Manager->deleteLastProductionLine(ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_ShowIssuerTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString tableName = gui->IssuerTableChoice->currentText();
  Logger->clear();

  Manager->showDatabaseTable(MatchingTable->value(tableName), IssuerModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_InitTransportMasterKeysPushButton_slot() {
  Logger->clear();

  Manager->initTransportMasterKeys(IssuerModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_InitIssuerTablePushButton_slot() {
  Logger->clear();

  Manager->initIssuers(IssuerModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_LinkIssuerWithKeysPushButton_slot() {
  QMap<QString, QString> linkParameters;
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString issuerId = gui->IssuerIdLineEdit1->text();
  QString masterKeysId = gui->MasterKeysLineEdit1->text();
  QString masterKeysType = gui->MasterKeysChoice->currentText();

  Logger->clear();

  // Проверка пользовательского ввода
  if (!checkLinkIssuerInput()) {
    Interactor->generateError(
        "Введены некорректные данные для связывания эмитента с ключами. ");
    return;
  }

  // Собираем параметры
  linkParameters.insert("issuer_id", issuerId);
  linkParameters.insert("master_keys_id", masterKeysId);
  linkParameters.insert("master_keys_type",
                        MatchingTable->value(masterKeysType));

  Manager->linkIssuerWithMasterKeys(IssuerModel, &linkParameters);

  CurrentGUI->update();
}

void MainWindowKernel::on_ApplySettingsPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QSettings settings;

  Logger->clear();

  // Проверка пользовательского ввода
  if (!checkNewSettings()) {
    Interactor->generateError("Введены некорректные данные для настроек. ");
    return;
  }

  // Считывание пользовательского ввода
  settings.setValue("Database/Server/Ip", gui->DatabaseIpLineEdit->text());
  settings.setValue("Database/Server/Port",
                    gui->DatabasePortLineEdit->text().toInt());
  settings.setValue("Database/Name", gui->DatabaseNameLineEdit->text());
  settings.setValue("Database/User/Name",
                    gui->DatabaseUserNameLineEdit->text());
  settings.setValue("Database/User/Password",
                    gui->DatabaseUserPasswordLineEdit->text());
  settings.setValue("Database/Log/Active",
                    gui->DatabaseLogOption->checkState() == Qt::Checked);

  settings.setValue("LogSystem/Save/Directory",
                    gui->LogSystemSavePathLineEdit->text());
  settings.setValue(
      "LogSystem/PersoServer/Enable",
      gui->LogSystemListenPersoServerCheckBox->checkState() == Qt::Checked
          ? true
          : false);
  settings.setValue("LogSystem/PersoServer/ListenIp",
                    gui->LogSystemListenIpLineEdit->text());
  settings.setValue("LogSystem/PersoServer/ListenPort",
                    gui->LogSystemListenPortLineEdit->text().toInt());

  // Применение новых настроек
  Manager->applySettings();

  // Оповещаем пользователя
  Interactor->generateNotification("Новые настройки успешно применены. ");
}

/*
 * Приватные методы
 */

void MainWindowKernel::proxyLogging(const QString& log) const {
  if (sender()->objectName() == QString("AdminManager"))
    emit logging(QString("Manager - ") + log);
  else
    emit logging(QString("Unknown - ") + log);
}

void MainWindowKernel::on_RequestMasterGui_slot(void) {
  createMasterGui();
}

void MainWindowKernel::loadSettings() const {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(PROGRAM_NAME);
}

bool MainWindowKernel::checkAuthorizationData() const {
  AuthorizationGUI* gui = dynamic_cast<AuthorizationGUI*>(CurrentGUI);

  QString login = gui->LoginLineEdit->text();
  QString password = gui->PasswordLineEdit->text();

  if ((login != MASTER_ACCESS_LOGIN) || (password != MASTER_ACCESS_PASSWORD)) {
    return false;
  }

  QSettings settings;
  settings.setValue("Authorization/Login", login);

  return true;
}

bool MainWindowKernel::checkNewSettings() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);

  QHostAddress ip = QHostAddress(gui->LogSystemListenIpLineEdit->text());
  if (ip.isNull()) {
    return false;
  }

  int32_t port = gui->LogSystemListenPortLineEdit->text().toInt();
  if ((port > IP_PORT_MAX_VALUE) || (port < IP_PORT_MIN_VALUE)) {
    return false;
  }

  ip = QHostAddress(gui->DatabaseIpLineEdit->text());
  if (ip.isNull()) {
    return false;
  }

  port = gui->DatabasePortLineEdit->text().toInt();
  if ((port > IP_PORT_MAX_VALUE) || (port < IP_PORT_MIN_VALUE)) {
    return false;
  }

  QFileInfo info(gui->LogSystemSavePathLineEdit->text());
  if (!info.isDir()) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkNewOrderInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);

  int32_t transponderQuantity =
      gui->TransponderQuantityLineEdit->text().toInt();
  int32_t boxCapacity = gui->BoxCapacityLineEdit->text().toInt();
  int32_t palletCapacity = gui->PalletCapacityLineEdit->text().toInt();
  QString transponderModel = gui->TransponderModelLineEdit->text();
  QString accrReference = gui->AccrReferenceLineEdit->text();
  QString equipmnetClass = gui->EquipmentClassLineEdit->text();
  QString manufacturerId = gui->ManufacturerIdLineEdit->text();
  QString panFilePath = gui->PanFilePathLineEdit->text();

  if (transponderQuantity <= 0) {
    return false;
  }

  if (boxCapacity <= 0) {
    return false;
  }

  if (palletCapacity <= 0) {
    return false;
  }

  if ((transponderQuantity % (boxCapacity * palletCapacity)) != 0) {
    return false;
  }

  if ((transponderModel.length() > TRANSPONDER_MODEL_CHAR_LENGTH) ||
      (transponderModel.length() == 0)) {
    return false;
  }

  QFileInfo info(gui->PanFilePathLineEdit->text());
  if ((!info.exists()) || (!info.isFile()) || (info.suffix() != "csv")) {
    return false;
  }

  QFile file(panFilePath);
  int32_t recordCount = 0;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString record = in.readLine();
    if (record.length() != PAN_CHAR_LENGTH) {
      file.close();
      return false;
    }
    recordCount++;
  }
  file.close();

  if (recordCount != transponderQuantity) {
    return false;
  }

  if ((accrReference.length() > ACCR_REFERENCE_CHAR_LENGTH) ||
      (accrReference.length() == 0)) {
    return false;
  }

  if ((equipmnetClass.length() > EQUIPMENT_CLASS_CHAR_LENGTH) ||
      (equipmnetClass.length() == 0)) {
    return false;
  }

  if ((manufacturerId.length() > MANUFACTURER_ID_CHAR_LENGTH) ||
      (manufacturerId.length() == 0)) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkNewProductionLineInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString login = gui->LoginLineEdit1->text();
  QString pass = gui->PasswordLineEdit1->text();

  if ((login.size() == 0) || (login.size() > 20)) {
    return false;
  }

  if ((pass.size() == 0) || (pass.size() > 20)) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkReleaseTransponderInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QRegularExpression regex("[A-Fa-f0-9]+");
  QString login = gui->LoginLineEdit2->text();
  QString pass = gui->PasswordLineEdit2->text();
  QString ucid = gui->UcidLineEdit->text();

  if (ucid.size() != UCID_CHAR_LENGTH) {
    return false;
  }

  QRegularExpressionMatch match = regex.match(ucid);
  if ((!match.hasMatch()) || (match.captured(0) != ucid)) {
    return false;
  }

  if ((login.size() == 0) || (login.size() > 20)) {
    return false;
  }

  if ((pass.size() == 0) || (pass.size() > 20)) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkSearchTransponderInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QRegularExpression ucidRegex("[A-Fa-f0-9]+");
  QRegularExpression panRegex("[0-9]+");
  QString choice = gui->SearchTransponderByComboBox->currentText();
  QString input = gui->SearchTransponderLineEdit->text();

  if (choice == "UCID") {
    if (input.size() != UCID_CHAR_LENGTH) {
      return false;
    }

    QRegularExpressionMatch match = ucidRegex.match(input);
    if ((!match.hasMatch()) || (match.captured(0) != input)) {
      return false;
    }
  } else if (choice == "SN") {
    if (input.toInt() == 0) {
      return false;
    }
  } else if (choice == "PAN") {
    if (input.length() != PAN_CHAR_LENGTH) {
      return false;
    }

    QRegularExpressionMatch match =
        panRegex.match(gui->SearchTransponderLineEdit->text());
    if ((!match.hasMatch()) || (match.captured(0) != input)) {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkConfirmRereleaseTransponderInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QRegularExpression ucidRegex("[A-Fa-f0-9]+");
  QRegularExpression panRegex("[0-9]+");
  QString choice = gui->RereleaseTransponderByComboBox->currentText();
  QString input = gui->RereleaseTransponderLineEdit->text();
  QString ucid = gui->NewUcidLineEdit->text();
  QString login = gui->LoginLineEdit3->text();
  QString pass = gui->PasswordLineEdit3->text();

  if ((login.size() == 0) || (login.size() > 20)) {
    return false;
  }

  if ((pass.size() == 0) || (pass.size() > 20)) {
    return false;
  }

  if (choice == "SN") {
    if (input.toInt() == 0) {
      return false;
    }
  } else if (choice == "PAN") {
    if (input.length() != PAN_CHAR_LENGTH) {
      return false;
    }

    QRegularExpressionMatch match = panRegex.match(input);
    if ((!match.hasMatch()) || (match.captured(0) != input)) {
      return false;
    }
  } else {
    return false;
  }

  if (ucid.size() != UCID_CHAR_LENGTH) {
    return false;
  }

  QRegularExpressionMatch match = ucidRegex.match(ucid);
  if ((!match.hasMatch()) || (match.captured(0) != ucid)) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkLinkIssuerInput() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString issuerId = gui->IssuerIdLineEdit1->text();
  QString masterKeysId = gui->MasterKeysLineEdit1->text();

  if (issuerId.toInt() == 0) {
    return false;
  }

  if (masterKeysId.toInt() == 0) {
    return false;
  }

  return true;
}

void MainWindowKernel::createTopMenu() {
  menuBar()->clear();
  createTopMenuActions();

  ServiceMenu = menuBar()->addMenu("Сервис");
  ServiceMenu->addAction(RequestAuthorizationGuiAct);

  HelpMenu = menuBar()->addMenu("Справка");
  HelpMenu->addAction(AboutProgramAct);
}

void MainWindowKernel::createTopMenuActions() {
  RequestAuthorizationGuiAct = new QAction("Авторизация");
  RequestAuthorizationGuiAct->setStatusTip(
      "Закрыть текущий интерфейс и создать начальный интерфейс");
  connect(RequestAuthorizationGuiAct, &QAction::triggered, this,
          &MainWindowKernel::on_RequestAuthorizationGuiAct_slot);

  AboutProgramAct = new QAction("О программе", this);
  AboutProgramAct->setStatusTip("Показать сведения о программе");
}

void MainWindowKernel::createAuthorazationGui() {
  // Удаляем предыдущий интерфейс
  if (CurrentGUI) {
    CurrentGUI->hide();
    delete CurrentGUI;
  }

  // Создаем виджеты
  CurrentGUI = new AuthorizationGUI(this);
  setCentralWidget(CurrentGUI);
  CurrentGUI->create();

  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);

  // Подключаем интерфейс
  connectAuthorizationGui();
}

void MainWindowKernel::connectAuthorizationGui() {
  AuthorizationGUI* gui = dynamic_cast<AuthorizationGUI*>(CurrentGUI);

  connect(gui->AuthorizePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_AuthorizePushButton_slot);
}

void MainWindowKernel::createMasterGui() {
  // Удаляем предыдущий интерфейс
  if (CurrentGUI) {
    CurrentGUI->hide();
    delete CurrentGUI;
  }
  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.8, DesktopGeometry.height() * 0.8);

  // Создаем интерфейс
  CurrentGUI = new MasterGUI(this);
  setCentralWidget(CurrentGUI);
  CurrentGUI->create();

  // Создаем верхнее меню
  createTopMenu();

  // Подключаем интерфейс
  connectMasterGui();
}

void MainWindowKernel::connectMasterGui() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);

  // База данных
  connect(gui->ShowDatabaseTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ShowDatabaseTablePushButton_slot);
  connect(gui->ClearDatabaseTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ClearDatabaseTablePushButton_slot);
  connect(gui->InitIssuerTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_InitIssuerTablePushButton_slot);

  connect(gui->TransmitCustomRequestPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_TransmitCustomRequestPushButton_slot);

  // Заказы
  connect(gui->CreateNewOrderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_CreateNewOrderPushButton_slot);
  connect(gui->StartOrderAssemblingPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_StartOrderAssemblingPushButton_slot);
  connect(gui->StopOrderAssemblingPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_StopOrderAssemblingPushButton_slot);
  connect(gui->UpdateOrderViewPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_UpdateOrderViewPushButton_slot);
  connect(gui->DeleteLastOrderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DeleteLastOrderPushButton_slot);

  // Производственные линии
  connect(gui->CreateNewProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_CreateNewProductionLinePushButton_slot);
  connect(gui->AllocateInactiveProductionLinesPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_AllocateInactiveProductionLinesPushButton_slot);
  connect(gui->LinkProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_LinkProductionLinePushButton_slot);
  connect(gui->DeactivateAllProductionLinesPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_DeactivateAllProductionLinesPushButton_slot);
  connect(gui->UpdateProductionLineViewPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_UpdateProductionLineViewPushButton_slot);
  connect(gui->DeleteLastProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DeleteLastProductionLinePushButton_slot);

  // Эмитенты
  connect(gui->ShowIssuerTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ShowIssuerTablePushButton_slot);
  connect(gui->InitTransportMasterKeysPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_InitTransportMasterKeysPushButton_slot);
  connect(gui->LinkIssuerWithKeysPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_LinkIssuerWithKeysPushButton_slot);

  // Сохранение настроек
  connect(gui->ApplySettingsPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ApplySettingsPushButton_slot);

  // Подключаем логгер
  connect(Logger, &LogSystem::requestDisplayLog, gui, &MasterGUI::displayLog);
  connect(Logger, &LogSystem::requestClearDisplayLog, gui,
          &MasterGUI::clearLogDisplay);

  // Соединяем модели и представления
  gui->DatabaseRandomModelView->setModel(RandomModel);
  gui->OrderTableView->setModel(OrderModel);
  gui->ProductionLineTableView->setModel(ProductionLineModel);
  gui->IssuerTableView->setModel(IssuerModel);

  // Связываем отображения графиков с логикой их формирования
}
void MainWindowKernel::createInterructor() {
  delete Interactor;
  Interactor = new UserInteractionSystem(this, this);
}

void MainWindowKernel::createManager(void) {
  delete Manager;
  Manager = new AdminManager(this);
  connect(Manager, &AdminManager::logging, this,
          &MainWindowKernel::proxyLogging);
  connect(Manager, &AdminManager::notifyUser, Interactor,
          &UserInteractionSystem::generateNotification);
  connect(Manager, &AdminManager::notifyUserAboutError, Interactor,
          &UserInteractionSystem::generateError);
  connect(Manager, &AdminManager::operationPerfomingStarted, Interactor,
          &UserInteractionSystem::generateProgressDialog);
  connect(Manager, &AdminManager::operationStepPerfomed, Interactor,
          &UserInteractionSystem::performeProgressDialogStep);
  connect(Manager, &AdminManager::operationPerformingFinished, Interactor,
          &UserInteractionSystem::completeProgressDialog);
  connect(Manager, &AdminManager::requestMasterGui_signal, this,
          &MainWindowKernel::on_RequestMasterGui_slot);
}

void MainWindowKernel::createLogger() {
  delete Logger;
  Logger = new LogSystem(this);
  connect(this, &MainWindowKernel::logging, Logger, &LogSystem::generate);
}

void MainWindowKernel::createModels() {
  RandomModel = new DatabaseTableModel(this);
  OrderModel = new DatabaseTableModel(this);
  ProductionLineModel = new DatabaseTableModel(this);
  IssuerModel = new DatabaseTableModel(this);
}

void MainWindowKernel::createMatchingTable() {
  MatchingTable = new QMap<QString, QString>;
  MatchingTable->insert("Транспортные мастер ключи", "transport_master_keys");
  MatchingTable->insert("Коммерческие мастер ключи", "commercial_master_keys");
  MatchingTable->insert("Заказчики", "issuers");
}
