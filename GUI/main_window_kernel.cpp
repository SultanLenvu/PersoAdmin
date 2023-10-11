#include "main_window_kernel.h"

MainWindowKernel::MainWindowKernel(QWidget* parent) : QMainWindow(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::desktop()->screenGeometry();

  // Загружаем пользовательские настройки
  loadSettings();

  // Графический интерфейс пока не создан
  CurrentGUI = nullptr;

  // Создаем систему логгирования
  createLoggerInstance();

  // Cистема взаимодействия с пользователем
  createInteractorInstance();

  // Управляющий модуль
  createManagerInstance();

  // Создаем модели для представлений
  createModels();

  // Создаем таблицу соответствий
  createMatchingTable();

  // Создаем графический интерфейс окна авторизации
  createAuthorazationGui();

  qRegisterMetaType<QSharedPointer<QMap<QString, QString>>>(
      "QSharedPointer<QMap<QString, QString> >");
}

MainWindowKernel::~MainWindowKernel() {
  ManagerThread->quit();
  ManagerThread->wait();

  LoggerThread->quit();
  LoggerThread->wait();
}

void MainWindowKernel::on_RequestAuthorizationGuiAct_slot() {
  createAuthorazationGui();
}

void MainWindowKernel::on_AuthorizePushButton_slot() {
  if (!checkAuthorizationData()) {
    Interactor->generateError("Неверный логин или пароль. ");
    return;
  }

  createMasterGui();

  emit connectDatabase_signal();
}

void MainWindowKernel::on_ConnectDatabasePushButton_slot() {
  emit loggerClear_signal();

  emit connectDatabase_signal();
}

void MainWindowKernel::on_DisconnectDatabasePushButton_slot() {
  emit loggerClear_signal();

  emit disconnectDatabase_signal();
}

void MainWindowKernel::on_ShowDatabaseTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  emit showDatabaseTable_signal(gui->DatabaseTableChoice->currentText(),
                                RandomModel);
}

void MainWindowKernel::on_ClearDatabaseTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  emit clearDatabaseTable_signal(gui->DatabaseTableChoice->currentText(),
                                 RandomModel);
}

void MainWindowKernel::on_TransmitCustomRequestPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  emit performCustomRequest_signal(gui->CustomRequestLineEdit->text(),
                                   RandomModel);
}

void MainWindowKernel::on_CreateNewOrderPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);

  emit loggerClear_signal();

  if (!checkNewOrderInput()) {
    Interactor->generateError("Некорректный ввод параметров нового заказа. ");
    return;
  }

  QSharedPointer<QMap<QString, QString>> orderParameters(
      new QMap<QString, QString>);
  orderParameters->insert("issuer_name",
                          gui->IssuerNameComboBox->currentText());
  orderParameters->insert("transponder_quantity",
                          gui->TransponderQuantityLineEdit->text());
  orderParameters->insert("box_capacity", gui->BoxCapacityLineEdit->text());
  orderParameters->insert("pallet_capacity",
                          gui->PalletCapacityLineEdit->text());
  orderParameters->insert(
      "full_personalization",
      gui->FullPersonalizationCheckBox->checkState() == Qt::Checked ? "true"
                                                                    : "false");
  orderParameters->insert("pan_file_path", gui->PanFilePathLineEdit->text());
  orderParameters->insert("transponder_model",
                          gui->TransponderModelLineEdit->text());
  orderParameters->insert("accr_reference", gui->AccrReferenceLineEdit->text());
  orderParameters->insert("equipment_class",
                          gui->EquipmentClassLineEdit->text());
  orderParameters->insert("manufacturer_id",
                          gui->ManufacturerIdLineEdit->text());

  emit createNewOrder_signal(orderParameters, OrderModel);
}

void MainWindowKernel::on_StartOrderAssemblingPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit startOrderAssembling_signal(gui->OrderIdLineEdit1->text(), OrderModel);
}

void MainWindowKernel::on_StopOrderAssemblingPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit stopOrderAssembling_signal(gui->OrderIdLineEdit1->text(), OrderModel);
}

void MainWindowKernel::on_UpdateOrderViewPushButton_slot() {
  emit loggerClear_signal();

  emit showDatabaseTable_signal("orders", OrderModel);
}

void MainWindowKernel::on_DeleteLastOrderPushButton_slot() {
  emit loggerClear_signal();

  emit deleteLastOrder_signal(OrderModel);
}

void MainWindowKernel::on_CreateNewProductionLinePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (!checkNewProductionLineInput()) {
    Interactor->generateError(
        "Некорректный ввод параметров нового производственной линии. ");
    return;
  }

  QMap<QString, QString> productionLineParameters;
  productionLineParameters.insert("login", gui->LoginLineEdit1->text());
  productionLineParameters.insert("password", gui->PasswordLineEdit1->text());
  emit createNewProductionLine_signal(&productionLineParameters,
                                      ProductionLineModel);

  CurrentGUI->update();
}

void MainWindowKernel::on_AllocateInactiveProductionLinesPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->OrderIdLineEdit2->text().toInt() == 0) {
    Interactor->generateError("Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit allocateInactiveProductionLines_signal(gui->OrderIdLineEdit2->text(),
                                              ProductionLineModel);
}

void MainWindowKernel::on_LinkProductionLinePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if ((!checkNewProductionLineInput()) ||
      (gui->BoxIdLineEdit1->text().toInt() == 0)) {
    Interactor->generateError(
        "Некорректный ввод параметров производственной линии. ");
    return;
  }

  QMap<QString, QString> linkParameters;
  linkParameters.insert("login", gui->LoginLineEdit1->text());
  linkParameters.insert("password", gui->PasswordLineEdit1->text());
  linkParameters.insert("box_id", gui->BoxIdLineEdit1->text());

  emit linkProductionLineWithBox_signal(&linkParameters, ProductionLineModel);
}

void MainWindowKernel::on_DeactivateAllProductionLinesPushButton_slot() {
  emit loggerClear_signal();

  emit shutdownAllProductionLines_signal(ProductionLineModel);
}

void MainWindowKernel::on_UpdateProductionLineViewPushButton_slot() {
  emit loggerClear_signal();

  emit showDatabaseTable_signal("production_lines", ProductionLineModel);
}

void MainWindowKernel::on_DeleteLastProductionLinePushButton_slot() {
  emit loggerClear_signal();

  emit deleteLastProductionLine_signal(ProductionLineModel);
}

void MainWindowKernel::on_ShowIssuerTablePushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString tableName = gui->IssuerTableChoice->currentText();
  emit loggerClear_signal();

  emit showDatabaseTable_signal(MatchingTable->value(tableName), IssuerModel);
}

void MainWindowKernel::on_InitTransportMasterKeysPushButton_slot() {
  emit loggerClear_signal();

  emit initTransportMasterKeys_signal(IssuerModel);
}

void MainWindowKernel::on_InitIssuerTablePushButton_slot() {
  emit loggerClear_signal();

  emit initIssuers_signal(IssuerModel);
}

void MainWindowKernel::on_LinkIssuerWithKeysPushButton_slot() {
  QMap<QString, QString> linkParameters;
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QString issuerId = gui->IssuerIdLineEdit1->text();
  QString masterKeysId = gui->MasterKeysLineEdit1->text();
  QString masterKeysType = gui->MasterKeysChoice->currentText();

  emit loggerClear_signal();

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

  emit linkIssuerWithMasterKeys_signal(IssuerModel, &linkParameters);
}

void MainWindowKernel::on_PrintTransponderStickerPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->TransponderIdLineEdit->text().toUInt() == 0) {
    Interactor->generateError(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printTransponderSticker_signal(gui->TransponderIdLineEdit->text(),
                                      StickerModel);
}

void MainWindowKernel::on_PrintBoxStickerPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->BoxIdLineEdit2->text().toUInt() == 0) {
    Interactor->generateError(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printBoxSticker_signal(gui->BoxIdLineEdit2->text(), StickerModel);
}

void MainWindowKernel::on_PrintPalletStickerPushButton_slot() {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  emit loggerClear_signal();

  if (gui->PalletIdLineEdit->text().toUInt() == 0) {
    Interactor->generateError(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printPalletSticker_signal(gui->PalletIdLineEdit->text(), StickerModel);
}

void MainWindowKernel::on_ApplySettingsPushButton_slot() {
  emit loggerClear_signal();

  // Проверка пользовательского ввода
  if (!checkNewSettings()) {
    Interactor->generateError("Введены некорректные данные для настроек. ");
    return;
  }

  // Сохранение настроек
  saveSettings();

  // Применение новых настроек
  emit applySettings_signal();

  // Оповещаем пользователя
  Interactor->generateNotification("Новые настройки успешно применены. ");
}

/*
 * Приватные методы
 */

void MainWindowKernel::loadSettings() const {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(PROGRAM_NAME);

  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                     QCoreApplication::applicationDirPath());
}

void MainWindowKernel::saveSettings() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QSettings settings;

  // Настройки системы администрирования
  settings.setValue(
      "administration_system/log_enable",
      gui->AdministrationSystemLogEnable->checkState() == Qt::Checked);

  // Настройки системы взаимодействия с пользователем
  settings.setValue(
      "user_interaction_system/log_enable",
      gui->UserInteractionSystemLogEnable->checkState() == Qt::Checked);

  // Настройки системы логгирования
  settings.setValue(
      "log_system/global_enable",
      gui->LogSystemEnableCheckBox->checkState() == Qt::Checked ? true : false);
  settings.setValue("log_system/save_directory",
                    gui->LogSystemSavePathLineEdit->text());
  settings.setValue(
      "log_system/udp_log_enable",
      gui->LogSystemListenPersoServerCheckBox->checkState() == Qt::Checked
          ? true
          : false);
  settings.setValue("log_system/udp_bind_ip",
                    gui->LogSystemListenIpLineEdit->text());
  settings.setValue("log_system/udp_bind_port",
                    gui->LogSystemListenPortLineEdit->text().toInt());

  // Настройки контроллера базы данных
  settings.setValue("postgres_controller/server_ip",
                    gui->DatabaseIpLineEdit->text());
  settings.setValue("postgres_controller/server_port",
                    gui->DatabasePortLineEdit->text().toInt());
  settings.setValue("postgres_controller/database_name",
                    gui->DatabaseNameLineEdit->text());
  settings.setValue("postgres_controller/user_name",
                    gui->DatabaseUserNameLineEdit->text());
  settings.setValue("postgres_controller/user_password",
                    gui->DatabaseUserPasswordLineEdit->text());
  settings.setValue(
      "postgres_controller/log_enable",
      gui->IDatabaseControllerLogEnable->checkState() == Qt::Checked);

  // Принтер стикеров
  settings.setValue("sticker_printer/library_path",
                    gui->StickerPrinterLibPathLineEdit->text());
}

bool MainWindowKernel::checkAuthorizationData() const {
  AuthorizationGUI* gui = dynamic_cast<AuthorizationGUI*>(CurrentGUI);

  QString login = gui->LoginLineEdit->text();
  QString password = gui->PasswordLineEdit->text();

  if ((login != MASTER_ACCESS_LOGIN) || (password != MASTER_ACCESS_PASSWORD)) {
    return false;
  }

  QSettings settings;
  settings.setValue("authorization/login", login);

  return true;
}

bool MainWindowKernel::checkNewSettings() const {
  MasterGUI* gui = dynamic_cast<MasterGUI*>(CurrentGUI);
  QFileInfo info;

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

  info.setFile(gui->LogSystemSavePathLineEdit->text());
  if (!info.isDir()) {
    return false;
  }

  info.setFile(gui->StickerPrinterLibPathLineEdit->text());
  if (!info.isFile()) {
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
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.1);

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
  connect(gui->ConnectDatabasePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ConnectDatabasePushButton_slot);
  connect(gui->DisconnectDatabasePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DisconnectDatabasePushButton_slot);
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

  // Стикеры
  connect(gui->PrintTransponderStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintTransponderStickerPushButton_slot);
  connect(gui->PrintBoxStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintBoxStickerPushButton_slot);
  connect(gui->PrintPalletStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintPalletStickerPushButton_slot);

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
  gui->StickerDataTableView->setModel(StickerModel);

  // Связываем отображения графиков с логикой их формирования
}

void MainWindowKernel::createLoggerInstance() {
  Logger = new LogSystem(nullptr);
  connect(this, &MainWindowKernel::applySettings_signal, Logger,
          &LogSystem::applySettings);
  connect(this, &MainWindowKernel::loggerClear_signal, Logger,
          &LogSystem::clear);
  connect(this, &MainWindowKernel::loggerGenerate_signal, Logger,
          &LogSystem::generate);

  LoggerThread = new QThread(this);
  connect(LoggerThread, &QThread::finished, LoggerThread,
          &QThread::deleteLater);
  connect(LoggerThread, &QThread::finished, Logger, &LogSystem::deleteLater);

  Logger->moveToThread(LoggerThread);
  LoggerThread->start();
}

void MainWindowKernel::createManagerInstance() {
  Manager = new AdminManager(nullptr);
  connect(Manager, &AdminManager::logging, Logger, &LogSystem::generate);
  connect(Manager, &AdminManager::notifyUser, Interactor,
          &UserInteractionSystem::generateNotification);
  connect(Manager, &AdminManager::notifyUserAboutError, Interactor,
          &UserInteractionSystem::generateError);
  connect(Manager, &AdminManager::operationPerfomingStarted, Interactor,
          &UserInteractionSystem::startOperationProgressDialog);
  connect(Manager, &AdminManager::operationPerformingFinished, Interactor,
          &UserInteractionSystem::finishOperationProgressDialog);

  // Подключаем функционал
  connect(this, &MainWindowKernel::applySettings_signal, Manager,
          &AdminManager::applySettings);
  connect(this, &MainWindowKernel::connectDatabase_signal, Manager,
          &AdminManager::connectDatabase);
  connect(this, &MainWindowKernel::disconnectDatabase_signal, Manager,
          &AdminManager::disconnectDatabase);
  connect(this, &MainWindowKernel::showDatabaseTable_signal, Manager,
          &AdminManager::showDatabaseTable);
  connect(this, &MainWindowKernel::clearDatabaseTable_signal, Manager,
          &AdminManager::clearDatabaseTable);
  connect(this, &MainWindowKernel::performCustomRequest_signal, Manager,
          &AdminManager::performCustomRequest);
  connect(this, &MainWindowKernel::createNewOrder_signal, Manager,
          &AdminManager::createNewOrder);
  connect(this, &MainWindowKernel::startOrderAssembling_signal, Manager,
          &AdminManager::startOrderAssembling);
  connect(this, &MainWindowKernel::stopOrderAssembling_signal, Manager,
          &AdminManager::stopOrderAssembling);
  connect(this, &MainWindowKernel::deleteLastOrder_signal, Manager,
          &AdminManager::deleteLastOrder);
  connect(this, &MainWindowKernel::showOrderTable_signal, Manager,
          &AdminManager::showOrderTable);
  connect(this, &MainWindowKernel::createNewProductionLine_signal, Manager,
          &AdminManager::createNewProductionLine);
  connect(this, &MainWindowKernel::allocateInactiveProductionLines_signal,
          Manager, &AdminManager::allocateInactiveProductionLines);
  connect(this, &MainWindowKernel::shutdownAllProductionLines_signal, Manager,
          &AdminManager::shutdownAllProductionLines);
  connect(this, &MainWindowKernel::deleteLastProductionLine_signal, Manager,
          &AdminManager::deleteLastProductionLine);
  connect(this, &MainWindowKernel::showProductionLineTable_signal, Manager,
          &AdminManager::showProductionLineTable);
  connect(this, &MainWindowKernel::linkProductionLineWithBox_signal, Manager,
          &AdminManager::linkProductionLineWithBox);
  connect(this, &MainWindowKernel::initIssuers_signal, Manager,
          &AdminManager::initIssuers);
  connect(this, &MainWindowKernel::initTransportMasterKeys_signal, Manager,
          &AdminManager::initTransportMasterKeys);
  connect(this, &MainWindowKernel::linkIssuerWithMasterKeys_signal, Manager,
          &AdminManager::linkIssuerWithMasterKeys);
  connect(this, &MainWindowKernel::printTransponderSticker_signal, Manager,
          &AdminManager::printTransponderSticker);
  connect(this, &MainWindowKernel::printBoxSticker_signal, Manager,
          &AdminManager::printBoxSticker);
  connect(this, &MainWindowKernel::printPalletSticker_signal, Manager,
          &AdminManager::printPalletSticker);

  ManagerThread = new QThread(this);
  connect(ManagerThread, &QThread::finished, ManagerThread,
          &QThread::deleteLater);
  connect(ManagerThread, &QThread::finished, Manager,
          &AdminManager::deleteLater);
  connect(ManagerThread, &QThread::started, Manager,
          &AdminManager::on_InsctanceThreadStarted);

  Manager->moveToThread(ManagerThread);
  ManagerThread->start();
}

void MainWindowKernel::createInteractorInstance() {
  Interactor = new UserInteractionSystem(CurrentGUI);
  connect(Interactor, &UserInteractionSystem::logging, Logger,
          &LogSystem::generate);
  connect(this, &MainWindowKernel::applySettings_signal, Interactor,
          &UserInteractionSystem::applySettings);
}

void MainWindowKernel::createModels() {
  RandomModel = new DatabaseTableModel(this);
  OrderModel = new DatabaseTableModel(this);
  ProductionLineModel = new DatabaseTableModel(this);
  IssuerModel = new DatabaseTableModel(this);
  StickerModel = new DatabaseTableModel(this);
}

void MainWindowKernel::createMatchingTable() {
  MatchingTable = new QMap<QString, QString>;
  MatchingTable->insert("Транспортные мастер ключи", "transport_master_keys");
  MatchingTable->insert("Коммерческие мастер ключи", "commercial_master_keys");
  MatchingTable->insert("Эмитенты", "issuers");
}
