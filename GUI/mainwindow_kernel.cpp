#include "mainwindow_kernel.h"

MainWindowKernel::MainWindowKernel(QWidget* parent) : QMainWindow(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::desktop()->screenGeometry();

  // Загружаем пользовательские настройки
  loadSettings();

  // Графический интерфейс пока не создан
  CurrentAbstractGUI = nullptr;

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
  createAuthorazationAbstractGUI();

  // Регистрируем пользовательские типы в мета-объеткной системе
  registerMetaType();
}

MainWindowKernel::~MainWindowKernel() {
  ManagerThread->quit();
  ManagerThread->wait();

  LoggerThread->quit();
  LoggerThread->wait();
}

void MainWindowKernel::on_RequestAuthorizationAbstractGUIAct_slot() {
  createAuthorazationAbstractGUI();
}

void MainWindowKernel::on_AuthorizePushButton_slot() {
  if (!checkAuthorizationData()) {
    Interactor->generateErrorMessage("Неверный логин или пароль. ");
    return;
  }

  createMainWindowGUI();

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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  emit showDatabaseTable_signal(AbstractGUI->DatabaseTableChoice->currentText(),
                                RandomModel);
}

void MainWindowKernel::on_ClearDatabaseTablePushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  emit clearDatabaseTable_signal(AbstractGUI->DatabaseTableChoice->currentText(),
                                 RandomModel);
}

void MainWindowKernel::on_TransmitCustomRequestPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  emit performCustomRequest_signal(AbstractGUI->CustomRequestLineEdit->text(),
                                   RandomModel);
}

void MainWindowKernel::on_CreateNewOrderPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (!checkNewOrderInput()) {
    Interactor->generateErrorMessage(
        "Некорректный ввод параметров нового заказа. ");
    return;
  }

  QSharedPointer<QHash<QString, QString>> orderParameters(
      new QHash<QString, QString>);
  orderParameters->insert("issuer_name",
                          AbstractGUI->IssuerNameComboBox->currentText());
  orderParameters->insert("transponder_quantity",
                          AbstractGUI->TransponderQuantityLineEdit->text());
  orderParameters->insert("box_capacity", AbstractGUI->BoxCapacityLineEdit->text());
  orderParameters->insert("pallet_capacity",
                          AbstractGUI->PalletCapacityLineEdit->text());
  orderParameters->insert(
      "full_personalization",
      AbstractGUI->FullPersonalizationCheckBox->checkState() == Qt::Checked ? "true"
                                                                    : "false");
  orderParameters->insert("pan_file_path", AbstractGUI->PanFilePathLineEdit->text());
  orderParameters->insert("transponder_model",
                          AbstractGUI->TransponderModelLineEdit->text());
  orderParameters->insert("accr_reference", AbstractGUI->AccrReferenceLineEdit->text());
  orderParameters->insert("equipment_class",
                          AbstractGUI->EquipmentClassLineEdit->text());
  orderParameters->insert("manufacturer_id",
                          AbstractGUI->ManufacturerIdLineEdit->text());

  emit createNewOrder_signal(orderParameters, OrderModel);
}

void MainWindowKernel::on_StartOrderAssemblingPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (AbstractGUI->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateErrorMessage(
        "Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit startOrderAssembling_signal(AbstractGUI->OrderIdLineEdit1->text(), OrderModel);
}

void MainWindowKernel::on_StopOrderAssemblingPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (AbstractGUI->OrderIdLineEdit1->text().toInt() == 0) {
    Interactor->generateErrorMessage(
        "Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit stopOrderAssembling_signal(AbstractGUI->OrderIdLineEdit1->text(), OrderModel);
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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (!checkNewProductionLineInput()) {
    Interactor->generateErrorMessage(
        "Некорректный ввод параметров нового производственной линии. ");
    return;
  }

  QHash<QString, QString> productionLineParameters;
  productionLineParameters.insert("login", AbstractGUI->LoginLineEdit1->text());
  productionLineParameters.insert("password", AbstractGUI->PasswordLineEdit1->text());
  emit createNewProductionLine_signal(&productionLineParameters,
                                      ProductionLineModel);

  CurrentAbstractGUI->update();
}

void MainWindowKernel::on_AllocateInactiveProductionLinesPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (AbstractGUI->OrderIdLineEdit2->text().toInt() == 0) {
    Interactor->generateErrorMessage(
        "Некорректный ввод идентификатора заказа. ");
    return;
  }

  emit allocateInactiveProductionLines_signal(AbstractGUI->OrderIdLineEdit2->text(),
                                              ProductionLineModel);
}

void MainWindowKernel::on_LinkProductionLinePushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if ((!checkNewProductionLineInput()) ||
      (AbstractGUI->BoxIdLineEdit1->text().toInt() == 0)) {
    Interactor->generateErrorMessage(
        "Некорректный ввод параметров производственной линии. ");
    return;
  }

  QHash<QString, QString> linkParameters;
  linkParameters.insert("login", AbstractGUI->LoginLineEdit1->text());
  linkParameters.insert("password", AbstractGUI->PasswordLineEdit1->text());
  linkParameters.insert("box_id", AbstractGUI->BoxIdLineEdit1->text());

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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QString tableName = AbstractGUI->IssuerTableChoice->currentText();
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
  QHash<QString, QString> linkParameters;
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QString issuerId = AbstractGUI->IssuerIdLineEdit1->text();
  QString masterKeysId = AbstractGUI->MasterKeysLineEdit1->text();
  QString masterKeysType = AbstractGUI->MasterKeysChoice->currentText();

  emit loggerClear_signal();

  // Проверка пользовательского ввода
  if (!checkLinkIssuerInput()) {
    Interactor->generateErrorMessage(
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

void MainWindowKernel::on_ReleaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("login", AbstractGUI->LoginLineEdit2->text());
  param->insert("password", AbstractGUI->PasswordLineEdit2->text());

  emit releaseTransponder_signal(param);
}

void MainWindowKernel::on_ConfirmTransponderPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("login", AbstractGUI->LoginLineEdit2->text());
  param->insert("password", AbstractGUI->PasswordLineEdit2->text());
  param->insert("ucid", AbstractGUI->UcidLineEdit->text());

  emit confirmTransponderRelease_signal(param);
}

void MainWindowKernel::on_RereleaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("login", AbstractGUI->LoginLineEdit2->text());
  param->insert("password", AbstractGUI->PasswordLineEdit2->text());
  param->insert("pan", AbstractGUI->RereleaseKeyLineEdit->text());

  emit rereleaseTransponder_signal(param);
}

void MainWindowKernel::on_ConfirmRereleaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("login", AbstractGUI->LoginLineEdit2->text());
  param->insert("password", AbstractGUI->PasswordLineEdit2->text());
  param->insert("pan", AbstractGUI->RereleaseKeyLineEdit->text());
  param->insert("ucid", AbstractGUI->UcidLineEdit->text());

  emit confirmTransponderRerelease_signal(param);
}

void MainWindowKernel::on_ProductionLineRollbackPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("login", AbstractGUI->LoginLineEdit2->text());
  param->insert("password", AbstractGUI->PasswordLineEdit2->text());

  emit rollbackProductionLine_signal(param);
}

void MainWindowKernel::on_PrintBoxStickerOnServerPushButton_slot() {
  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  if (!Interactor->getPan(param.get())) {
    return;
  }

  if (param->isEmpty()) {
    Interactor->generateErrorMessage("Некорректный ввод данных");
    return;
  }

  emit printBoxStickerOnServer_signal(param);
}

void MainWindowKernel::on_PrintLastBoxStickerOnServerPushButton_slot() {
  emit loggerClear_signal();

  emit printLastBoxStickerOnServer_signal();
}

void MainWindowKernel::on_PrintPalletStickerOnServerPushButton_slot() {
  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  if (!Interactor->getPan(param.get())) {
    return;
  }

  if (param->isEmpty()) {
    Interactor->generateErrorMessage("Некорректный ввод данных");
    return;
  }

  emit printPalletStickerOnServer_signal(param);
}

void MainWindowKernel::on_PrintLastPalletStickerOnServerPushButton_slot() {
  emit printLastPalletStickerOnServer_signal();
}

void MainWindowKernel::on_TransponderManualReleasePushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (AbstractGUI->AnyIdLineEdit->text().toInt() == 0) {
    Interactor->generateErrorMessage("Некорректный ввод данных.");
    return;
  }

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("table",
                MatchingTable->value(AbstractGUI->ChoiceAnyIdComboBox->currentText()));
  param->insert("id", AbstractGUI->AnyIdLineEdit->text());
  emit releaseTranspondersManually_signal(param, TransponderModel);
}

void MainWindowKernel::on_TransponderManualRefundPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  if (AbstractGUI->AnyIdLineEdit->text().toInt() == 0) {
    Interactor->generateErrorMessage("Некорректный ввод данных.");
    return;
  }

  QSharedPointer<QHash<QString, QString>> param(new QHash<QString, QString>());
  param->insert("table",
                MatchingTable->value(AbstractGUI->ChoiceAnyIdComboBox->currentText()));
  param->insert("id", AbstractGUI->AnyIdLineEdit->text());
  emit refundTranspondersManually_signal(param, TransponderModel);
}

void MainWindowKernel::on_PalletShipmentPushButton_slot() {
  emit loggerClear_signal();

  QSharedPointer<QHash<QString, QString>> params(new QHash<QString, QString>());
  if (!Interactor->getPalletShipingParameters(params.get())) {
    return;
  }

  if (params->isEmpty()) {
    Interactor->generateErrorMessage("Некорректный ввод данных");
    return;
  }

  emit shipPallets_signal(params, TransponderModel);
}

void MainWindowKernel::on_PrintTransponderStickerPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  if (AbstractGUI->TransponderIdLineEdit->text().toUInt() == 0) {
    Interactor->generateErrorMessage(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printTransponderSticker_signal(AbstractGUI->TransponderIdLineEdit->text(),
                                      StickerModel);
}

void MainWindowKernel::on_PrintBoxStickerPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  if (AbstractGUI->BoxIdLineEdit2->text().toUInt() == 0) {
    Interactor->generateErrorMessage(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printBoxSticker_signal(AbstractGUI->BoxIdLineEdit2->text(), StickerModel);
}

void MainWindowKernel::on_PrintPalletStickerPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  emit loggerClear_signal();

  if (AbstractGUI->PalletIdLineEdit->text().toUInt() == 0) {
    Interactor->generateErrorMessage(
        "Введен некорректный идентификатор транспондера. ");
    return;
  }

  emit printPalletSticker_signal(AbstractGUI->PalletIdLineEdit->text(), StickerModel);
}

void MainWindowKernel::on_ExecStickerPrinterCommandScriptPushButton_slot() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  emit loggerClear_signal();

  QSharedPointer<QStringList> commandScript(new QStringList(
      AbstractGUI->StickerPrinterCommandScriptInput->toPlainText().split("\n")));
  emit execPrinterStickerCommandScript_signal(commandScript);
}

void MainWindowKernel::on_ApplySettingsPushButton_slot() {
  emit loggerClear_signal();

  // Проверка пользовательского ввода
  if (!checkNewSettings()) {
    Interactor->generateErrorMessage(
        "Введены некорректные данные для настроек. ");
    return;
  }

  // Сохранение настроек
  saveSettings();

  // Применение новых настроек
  emit applySettings_signal();

  // Оповещаем пользователя
  Interactor->generateMessage("Новые настройки успешно применены. ");
}

void MainWindowKernel::displayFirmware_slot(QSharedPointer<QFile> firmware) {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  if (!firmware.get()->open(QIODevice::ReadOnly)) {
    Interactor->generateErrorMessage(
        "Не удалось открыть файл прошивки для отображения.");
  }
  AbstractGUI->AssembledFirmwareView->setPlainText(firmware.get()->readAll().toHex());
  firmware.get()->close();
}

void MainWindowKernel::displayTransponderData_slot(
    QSharedPointer<QHash<QString, QString>> transponderData) {
  TransponderData->buildTransponderData(transponderData.get());
  CurrentAbstractGUI->update();
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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QSettings settings;

  // Настройки системы логгирования
  settings.setValue(
      "log_system/global_enable",
      AbstractGUI->LogSystemGlobalEnableCheckBox->checkState() == Qt::Checked ? true
                                                                      : false);
  settings.setValue(
      "log_system/extended_enable",
      AbstractGUI->LogSystemExtendedEnableCheckBox->checkState() == Qt::Checked
          ? true
          : false);
  settings.setValue(
      "log_system/display_log_enable",
      AbstractGUI->LogSystemDisplayEnableCheckBox->checkState() == Qt::Checked ? true
                                                                       : false);
  settings.setValue(
      "log_system/file_log_enable",
      AbstractGUI->LogSystemFileEnableCheckBox->checkState() == Qt::Checked ? true
                                                                    : false);
  settings.setValue("log_system/log_file_max_number",
                    AbstractGUI->LogSystemFileMaxNumberLineEdit->text());
  settings.setValue(
      "log_system/udp_listen_enable",
      AbstractGUI->LogSystemListenPersoServerCheckBox->checkState() == Qt::Checked
          ? true
          : false);
  settings.setValue("log_system/udp_listen_ip",
                    AbstractGUI->LogSystemListenIpLineEdit->text());
  settings.setValue("log_system/udp_listen_port",
                    AbstractGUI->LogSystemListenPortLineEdit->text().toInt());

  // Настройки клиента
  settings.setValue("perso_client/server_ip",
                    AbstractGUI->PersoClientServerIpLineEdit->text());
  settings.setValue("perso_client/server_port",
                    AbstractGUI->PersoClientServerPortLineEdit->text());

  // Настройки контроллера базы данных
  settings.setValue("postgres_controller/server_ip",
                    AbstractGUI->DatabaseIpLineEdit->text());
  settings.setValue("postgres_controller/server_port",
                    AbstractGUI->DatabasePortLineEdit->text().toInt());
  settings.setValue("postgres_controller/database_name",
                    AbstractGUI->DatabaseNameLineEdit->text());
  settings.setValue("postgres_controller/user_name",
                    AbstractGUI->DatabaseUserNameLineEdit->text());
  settings.setValue("postgres_controller/user_password",
                    AbstractGUI->DatabaseUserPasswordLineEdit->text());
  settings.setValue(
      "postgres_controller/log_enable",
      AbstractGUI->IDatabaseControllerLogEnable->checkState() == Qt::Checked);

  // Принтер стикеров
  settings.setValue("sticker_printer/library_path",
                    AbstractGUI->StickerPrinterLibPathLineEdit->text());
  settings.setValue("sticker_printer/name",
                    AbstractGUI->StickerPrinterNameLineEdit->text());
}

bool MainWindowKernel::checkAuthorizationData() const {
  AuthorizationAbstractGUI* AbstractGUI = dynamic_cast<AuthorizationAbstractGUI*>(CurrentAbstractGUI);

  QString login = AbstractGUI->LoginLineEdit->text();
  QString password = AbstractGUI->PasswordLineEdit->text();

  if ((login != MASTER_ACCESS_LOGIN) || (password != MASTER_ACCESS_PASSWORD)) {
    return false;
  }

  QSettings settings;
  settings.setValue("authorization/login", login);

  return true;
}

bool MainWindowKernel::checkNewSettings() const {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QFileInfo info;
  QHostAddress ip;
  int32_t port;

  if (AbstractGUI->LogSystemGlobalEnableCheckBox->checkState() == Qt::Unchecked) {
    return true;
  }

  if (AbstractGUI->LogSystemListenPersoServerCheckBox->checkState() == Qt::Checked) {
    ip = QHostAddress(AbstractGUI->LogSystemListenIpLineEdit->text());
    if (ip.isNull()) {
      return false;
    }

    port = AbstractGUI->LogSystemListenPortLineEdit->text().toInt();
    if ((port > IP_PORT_MAX_VALUE) || (port < IP_PORT_MIN_VALUE)) {
      return false;
    }
  }

  ip = QHostAddress(AbstractGUI->DatabaseIpLineEdit->text());
  if (ip.isNull()) {
    return false;
  }

  ip = QHostAddress(AbstractGUI->PersoClientServerIpLineEdit->text());
  if (ip.isNull()) {
    return false;
  }

  port = AbstractGUI->PersoClientServerPortLineEdit->text().toInt();
  if ((port > IP_PORT_MAX_VALUE) || (port < IP_PORT_MIN_VALUE)) {
    return false;
  }

  if (AbstractGUI->LogSystemFileEnableCheckBox->checkState() == Qt::Checked) {
    port = AbstractGUI->DatabasePortLineEdit->text().toInt();
    if ((port > IP_PORT_MAX_VALUE) || (port < IP_PORT_MIN_VALUE)) {
      return false;
    }

    if (AbstractGUI->LogSystemFileMaxNumberLineEdit->text().toInt() == 0) {
      return false;
    }
  }

  info.setFile(AbstractGUI->StickerPrinterLibPathLineEdit->text());
  if (!info.isFile()) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkNewOrderInput() const {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  int32_t transponderQuantity =
      AbstractGUI->TransponderQuantityLineEdit->text().toInt();
  int32_t boxCapacity = AbstractGUI->BoxCapacityLineEdit->text().toInt();
  int32_t palletCapacity = AbstractGUI->PalletCapacityLineEdit->text().toInt();
  QString transponderModel = AbstractGUI->TransponderModelLineEdit->text();
  QString accrReference = AbstractGUI->AccrReferenceLineEdit->text();
  QString equipmnetClass = AbstractGUI->EquipmentClassLineEdit->text();
  QString manufacturerId = AbstractGUI->ManufacturerIdLineEdit->text();
  QString panFilePath = AbstractGUI->PanFilePathLineEdit->text();

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

  QFileInfo info(AbstractGUI->PanFilePathLineEdit->text());
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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QString login = AbstractGUI->LoginLineEdit1->text();
  QString pass = AbstractGUI->PasswordLineEdit1->text();

  if ((login.size() == 0) || (login.size() > 20)) {
    return false;
  }

  if ((pass.size() == 0) || (pass.size() > 20)) {
    return false;
  }

  return true;
}

bool MainWindowKernel::checkReleaseTransponderInput() const {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QRegularExpression regex("[A-Fa-f0-9]+");
  QString login = AbstractGUI->LoginLineEdit2->text();
  QString pass = AbstractGUI->PasswordLineEdit2->text();
  QString ucid = AbstractGUI->UcidLineEdit->text();

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

bool MainWindowKernel::checkConfirmRereleaseTransponderInput() const {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QRegularExpression ucidRegex("[A-Fa-f0-9]+");
  QRegularExpression panRegex("[0-9]+");
  QString choice = AbstractGUI->RereleaseKeyComboBox->currentText();
  QString input = AbstractGUI->RereleaseKeyLineEdit->text();
  QString ucid = AbstractGUI->UcidLineEdit->text();
  QString login = AbstractGUI->LoginLineEdit2->text();
  QString pass = AbstractGUI->PasswordLineEdit2->text();

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
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);
  QString issuerId = AbstractGUI->IssuerIdLineEdit1->text();
  QString masterKeysId = AbstractGUI->MasterKeysLineEdit1->text();

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
  ServiceMenu->addAction(RequestAuthorizationAbstractGUIAct);

  HelpMenu = menuBar()->addMenu("Справка");
  HelpMenu->addAction(AboutProgramAct);
}

void MainWindowKernel::createTopMenuActions() {
  RequestAuthorizationAbstractGUIAct = new QAction("Авторизация");
  RequestAuthorizationAbstractGUIAct->setStatusTip(
      "Закрыть текущий интерфейс и создать начальный интерфейс");
  connect(RequestAuthorizationAbstractGUIAct, &QAction::triggered, this,
          &MainWindowKernel::on_RequestAuthorizationAbstractGUIAct_slot);

  AboutProgramAct = new QAction("О программе", this);
  AboutProgramAct->setStatusTip("Показать сведения о программе");
}

void MainWindowKernel::createAuthorazationAbstractGUI() {
  // Удаляем предыдущий интерфейс
  if (CurrentAbstractGUI) {
    CurrentAbstractGUI->hide();
    delete CurrentAbstractGUI;
  }

  // Создаем виджеты
  CurrentAbstractGUI = new AuthorizationAbstractGUI(this);
  setCentralWidget(CurrentAbstractGUI);
  CurrentAbstractGUI->create();

  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.1);

  // Подключаем интерфейс
  connectAuthorizationAbstractGUI();
}

void MainWindowKernel::connectAuthorizationAbstractGUI() {
  AuthorizationAbstractGUI* AbstractGUI = dynamic_cast<AuthorizationAbstractGUI*>(CurrentAbstractGUI);

  connect(AbstractGUI->AuthorizePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_AuthorizePushButton_slot);
}

void MainWindowKernel::createMainWindowGUI() {
  // Удаляем предыдущий интерфейс
  if (CurrentAbstractGUI) {
    CurrentAbstractGUI->hide();
    delete CurrentAbstractGUI;
  }
  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.8, DesktopGeometry.height() * 0.8);

  // Создаем интерфейс
  CurrentAbstractGUI = new MainWindowGUI(this);
  setCentralWidget(CurrentAbstractGUI);
  CurrentAbstractGUI->create();

  // Создаем верхнее меню
  createTopMenu();

  // Подключаем интерфейс
  connectMainWindowGUI();
}

void MainWindowKernel::connectMainWindowGUI() {
  MainWindowGUI* AbstractGUI = dynamic_cast<MainWindowGUI*>(CurrentAbstractGUI);

  // База данных
  connect(AbstractGUI->ConnectDatabasePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ConnectDatabasePushButton_slot);
  connect(AbstractGUI->DisconnectDatabasePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DisconnectDatabasePushButton_slot);
  connect(AbstractGUI->ShowDatabaseTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ShowDatabaseTablePushButton_slot);
  connect(AbstractGUI->ClearDatabaseTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ClearDatabaseTablePushButton_slot);
  connect(AbstractGUI->InitIssuerTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_InitIssuerTablePushButton_slot);

  connect(AbstractGUI->TransmitCustomRequestPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_TransmitCustomRequestPushButton_slot);

  // Заказы
  connect(AbstractGUI->CreateNewOrderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_CreateNewOrderPushButton_slot);
  connect(AbstractGUI->StartOrderAssemblingPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_StartOrderAssemblingPushButton_slot);
  connect(AbstractGUI->StopOrderAssemblingPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_StopOrderAssemblingPushButton_slot);
  connect(AbstractGUI->UpdateOrderViewPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_UpdateOrderViewPushButton_slot);
  connect(AbstractGUI->DeleteLastOrderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DeleteLastOrderPushButton_slot);

  // Производственные линии
  connect(AbstractGUI->CreateNewProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_CreateNewProductionLinePushButton_slot);
  connect(AbstractGUI->AllocateInactiveProductionLinesPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_AllocateInactiveProductionLinesPushButton_slot);
  connect(AbstractGUI->LinkProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_LinkProductionLinePushButton_slot);
  connect(AbstractGUI->DeactivateAllProductionLinesPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_DeactivateAllProductionLinesPushButton_slot);
  connect(AbstractGUI->UpdateProductionLineViewPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_UpdateProductionLineViewPushButton_slot);
  connect(AbstractGUI->DeleteLastProductionLinePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_DeleteLastProductionLinePushButton_slot);

  // Эмитенты
  connect(AbstractGUI->ShowIssuerTablePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ShowIssuerTablePushButton_slot);
  connect(AbstractGUI->InitTransportMasterKeysPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_InitTransportMasterKeysPushButton_slot);
  connect(AbstractGUI->LinkIssuerWithKeysPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_LinkIssuerWithKeysPushButton_slot);

  // Тест сервера
  connect(AbstractGUI->ReleaseTransponderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ReleaseTransponderPushButton_slot);
  connect(AbstractGUI->ConfirmTransponderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ConfirmTransponderPushButton_slot);
  connect(AbstractGUI->RereleaseTransponderPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_RereleaseTransponderPushButton_slot);
  connect(AbstractGUI->ConfirmRereleaseTransponderPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_ConfirmRereleaseTransponderPushButton_slot);
  connect(AbstractGUI->ProductionLineRollbackPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ProductionLineRollbackPushButton_slot);

  connect(AbstractGUI->PrintBoxStickerOnServerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintBoxStickerOnServerPushButton_slot);
  connect(AbstractGUI->PrintLastBoxStickerOnServerPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_PrintLastBoxStickerOnServerPushButton_slot);
  connect(AbstractGUI->PrintPalletStickerOnServerPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_PrintPalletStickerOnServerPushButton_slot);
  connect(AbstractGUI->PrintLastPalletStickerOnServerPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_PrintLastPalletStickerOnServerPushButton_slot);

  // Транспондеры
  connect(AbstractGUI->TransponderManualReleasePushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_TransponderManualReleasePushButton_slot);
  connect(AbstractGUI->TransponderManualRefundPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_TransponderManualRefundPushButton_slot);
  connect(AbstractGUI->PalletShipmentPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PalletShipmentPushButton_slot);

  // Стикеры
  connect(AbstractGUI->PrintTransponderStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintTransponderStickerPushButton_slot);
  connect(AbstractGUI->PrintBoxStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintBoxStickerPushButton_slot);
  connect(AbstractGUI->PrintPalletStickerPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_PrintPalletStickerPushButton_slot);
  connect(AbstractGUI->ExecStickerPrinterCommandScriptPushButton, &QPushButton::clicked,
          this,
          &MainWindowKernel::on_ExecStickerPrinterCommandScriptPushButton_slot);

  // Сохранение настроек
  connect(AbstractGUI->ApplySettingsPushButton, &QPushButton::clicked, this,
          &MainWindowKernel::on_ApplySettingsPushButton_slot);

  // Подключаем логгер
  connect(Logger->getWidgetLogger(), &WidgetLogBackend::displayLog_signal,
          AbstractGUI, &MainWindowGUI::displayLog);
  connect(Logger->getWidgetLogger(), &WidgetLogBackend::clearLogDisplay_signal,
          AbstractGUI, &MainWindowGUI::clearLogDisplay);

  // Соединяем модели и представления
  AbstractGUI->DatabaseRandomModelView->setModel(RandomModel);
  AbstractGUI->OrderTableView->setModel(OrderModel);
  AbstractGUI->ProductionLineTableView->setModel(ProductionLineModel);
  AbstractGUI->IssuerTableView->setModel(IssuerModel);
  AbstractGUI->StickerDataTableView->setModel(StickerModel);
  AbstractGUI->TransponderTableView->setModel(TransponderModel);

  AbstractGUI->TransponderDataTableView->setModel(TransponderData);

  // Связываем отображения графиков с логикой их формирования
}

void MainWindowKernel::createLoggerInstance() {
  Logger = LogSystem::instance();
  connect(this, &MainWindowKernel::applySettings_signal, Logger,
          &LogSystem::applySettings);
  connect(this, &MainWindowKernel::loggerClear_signal, Logger,
          &LogSystem::clear);
  connect(this, &MainWindowKernel::logging, Logger, &LogSystem::generate);

  LoggerThread = new QThread(this);
  connect(LoggerThread, &QThread::finished, LoggerThread,
          &QThread::deleteLater);

  Logger->moveToThread(LoggerThread);
  LoggerThread->start();
}

void MainWindowKernel::createManagerInstance() {
  Manager = new AdminManager(nullptr);
  connect(Manager, &AdminManager::logging, Logger, &LogSystem::generate);
  connect(Manager, &AdminManager::notifyUser, Interactor,
          &InteractionSystem::generateMessage);
  connect(Manager, &AdminManager::notifyUserAboutError, Interactor,
          &InteractionSystem::generateErrorMessage);
  connect(Manager, &AdminManager::operationPerfomingStarted, Interactor,
          &InteractionSystem::startOperationProgressDialog);
  connect(Manager, &AdminManager::operationPerformingFinished, Interactor,
          &InteractionSystem::finishOperationProgressDialog);

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

  connect(this, &MainWindowKernel::releaseTransponder_signal, Manager,
          &AdminManager::releaseTransponder);
  connect(this, &MainWindowKernel::confirmTransponderRelease_signal, Manager,
          &AdminManager::confirmTransponderRelease);
  connect(this, &MainWindowKernel::rereleaseTransponder_signal, Manager,
          &AdminManager::rereleaseTransponder);
  connect(this, &MainWindowKernel::confirmTransponderRerelease_signal, Manager,
          &AdminManager::confirmTransponderRerelease);
  connect(this, &MainWindowKernel::rollbackProductionLine_signal, Manager,
          &AdminManager::rollbackProductionLine);
  connect(this, &MainWindowKernel::printBoxStickerOnServer_signal, Manager,
          &AdminManager::printBoxStickerOnServer);
  connect(this, &MainWindowKernel::printLastBoxStickerOnServer_signal, Manager,
          &AdminManager::printLastBoxStickerOnServer);
  connect(this, &MainWindowKernel::printPalletStickerOnServer_signal, Manager,
          &AdminManager::printPalletStickerOnServer);
  connect(this, &MainWindowKernel::printLastPalletStickerOnServer_signal,
          Manager, &AdminManager::printLastPalletStickerOnServer);

  connect(this, &MainWindowKernel::releaseTranspondersManually_signal, Manager,
          &AdminManager::releaseTranspondersManually);
  connect(this, &MainWindowKernel::refundTranspondersManually_signal, Manager,
          &AdminManager::refundTranspondersManually);
  connect(this, &MainWindowKernel::shipPallets_signal, Manager,
          &AdminManager::shipPallets);

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
  connect(this, &MainWindowKernel::execPrinterStickerCommandScript_signal,
          Manager, &AdminManager::execPrinterStickerCommandScript);

  // Запросы на отображение
  connect(Manager, &AdminManager::displayFirmware_signal, this,
          &MainWindowKernel::displayFirmware_slot);
  connect(Manager, &AdminManager::displayTransponderData_signal, this,
          &MainWindowKernel::displayTransponderData_slot);

  // Поток
  ManagerThread = new QThread(this);
  connect(ManagerThread, &QThread::finished, ManagerThread,
          &QThread::deleteLater);
  connect(ManagerThread, &QThread::finished, Manager,
          &AdminManager::deleteLater);
  connect(ManagerThread, &QThread::started, Manager,
          &AdminManager::on_InsctanceThreadStarted_slot);

  Manager->moveToThread(ManagerThread);
  ManagerThread->start();
}

void MainWindowKernel::createInteractorInstance() {
  Interactor = InteractionSystem::instance();
  connect(Interactor, &InteractionSystem::logging, Logger,
          &LogSystem::generate);
  connect(this, &MainWindowKernel::applySettings_signal, Interactor,
          &InteractionSystem::applySettings);
}

void MainWindowKernel::createModels() {
  RandomModel = new DatabaseTableModel(this);
  OrderModel = new DatabaseTableModel(this);
  ProductionLineModel = new DatabaseTableModel(this);
  IssuerModel = new DatabaseTableModel(this);
  StickerModel = new DatabaseTableModel(this);
  TransponderModel = new DatabaseTableModel(this);

  TransponderData = new HashModel(this);
}

void MainWindowKernel::createMatchingTable() {
  MatchingTable = new QHash<QString, QString>;
  MatchingTable->insert("Транспортные мастер ключи", "transport_master_keys");
  MatchingTable->insert("Коммерческие мастер ключи", "commercial_master_keys");
  MatchingTable->insert("Эмитенты", "issuers");
  MatchingTable->insert("ID транспондера", "transponders");
  MatchingTable->insert("ID бокса", "boxes");
  MatchingTable->insert("ID паллеты", "pallets");
  MatchingTable->insert("ID заказа", "orders");
}

void MainWindowKernel::registerMetaType() {
  qRegisterMetaType<QSharedPointer<QMap<QString, QString>>>(
      "QSharedPointer<QHash<QString, QString> >");
  qRegisterMetaType<QSharedPointer<QHash<QString, QString>>>(
      "QSharedPointer<QHash<QString, QString> >");
  qRegisterMetaType<QSharedPointer<QStringList>>("QSharedPointer<QStringList>");
  qRegisterMetaType<QSharedPointer<QFile>>("QSharedPointer<QFile>");
}