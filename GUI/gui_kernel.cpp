#include "gui_kernel.h"
#include "authorization_gui.h"
#include "definitions.h"
#include "string_input_dialog.h"
#include "link_issuer_key_dialog.h"
#include "mainwindow_gui.h"
#include "manual_release_refund_dialog.h"
#include "order_creation_dialog.h"
#include "pallet_shiping_dialog.h"
#include "pan_input_dialog.h"
#include "production_line_creation_dialog.h"
#include "settings_dialog.h"

GuiKernel::GuiKernel(QWidget* parent) : QMainWindow(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

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
  createAuthorazationGUI();

  // Регистрируем пользовательские типы в мета-объеткной системе
  registerMetaType();
}

GuiKernel::~GuiKernel() {
  ManagerThread->quit();
  ManagerThread->wait();

  LoggerThread->quit();
  LoggerThread->wait();
}

void GuiKernel::requestAuthorizationGUIAct_slot() {
  createAuthorazationGUI();
}

void GuiKernel::authorizePushButton_slot() {
  createMainWindowGUI();

  emit connectDatabase_signal();
}

void GuiKernel::connectDatabasePushButton_slot() {
  emit loggerClear_signal();

  emit connectDatabase_signal();
}

void GuiKernel::disconnectDatabasePushButton_slot() {
  emit loggerClear_signal();

  emit disconnectDatabase_signal();
}

void GuiKernel::showDatabaseTablePushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);
  emit loggerClear_signal();

  emit showDatabaseTable_signal(AbstractGui->DatabaseTableChoice->currentText(),
                                RandomModel);
}

void GuiKernel::transmitCustomRequestPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);
  emit loggerClear_signal();

  emit performCustomRequest_signal(AbstractGui->CustomRequestLineEdit->text(),
                                   RandomModel);
}

void GuiKernel::createNewOrderPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  OrderCreationDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit createNewOrder_signal(param, OrderModel);
}

void GuiKernel::startOrderAssemblingPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit startOrderAssembling_signal(param, OrderModel);
}

void GuiKernel::stopOrderAssemblingPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit stopOrderAssembling_signal(param, OrderModel);
}

void GuiKernel::updateOrderViewPushButton_slot() {
  emit loggerClear_signal();
  emit showDatabaseTable_signal("orders", OrderModel);
}

void GuiKernel::createNewProductionLinePushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  ProductionLineCreationDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit createNewProductionLine_signal(param, ProductionLineModel);
}

void GuiKernel::activateProductionLinePushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit activateProductionLine_signal(param, ProductionLineModel);
}

void GuiKernel::activateAllProductionLinesPushButton_slot() {
  emit loggerClear_signal();
  emit activateAllProductionLines_signal(ProductionLineModel);
}

void GuiKernel::deactivateProductionLinePushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit deactivateProductionLine_signal(param, ProductionLineModel);
}

void GuiKernel::deactivateAllProductionLinesPushButton_slot() {
  emit loggerClear_signal();
  emit deactivateAllProductionLines_signal(ProductionLineModel);
}

void GuiKernel::editProductionLinesPushButton_slot() {
  emit loggerClear_signal();
}

void GuiKernel::updateProductionLineViewPushButton_slot() {
  emit loggerClear_signal();
  emit showDatabaseTable_signal("production_lines", ProductionLineModel);
}

void GuiKernel::initTransportMasterKeysPushButton_slot() {
  emit loggerClear_signal();

  emit initTransportMasterKeys_signal(IssuerModel);
}

void GuiKernel::initIssuerTablePushButton_slot() {
  emit loggerClear_signal();

  emit initIssuers_signal(IssuerModel);
}

void GuiKernel::linkIssuerWithKeysPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  LinkIssuerKeyDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit linkIssuerWithMasterKeys_signal(param, IssuerModel);
}

void GuiKernel::releaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<StringDictionary> param(new StringDictionary());
  param->insert("login", AbstractGui->LoginLineEdit2->text());
  param->insert("password", AbstractGui->PasswordLineEdit2->text());

  emit releaseTransponder_signal(param);
}

void GuiKernel::confirmTransponderPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<StringDictionary> param(new StringDictionary());
  param->insert("login", AbstractGui->LoginLineEdit2->text());
  param->insert("password", AbstractGui->PasswordLineEdit2->text());
  param->insert("ucid", AbstractGui->UcidLineEdit->text());

  emit confirmTransponderRelease_signal(param);
}

void GuiKernel::rereleaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<StringDictionary> param(new StringDictionary());
  param->insert("login", AbstractGui->LoginLineEdit2->text());
  param->insert("password", AbstractGui->PasswordLineEdit2->text());
  param->insert("pan", AbstractGui->RereleaseKeyLineEdit->text());

  emit rereleaseTransponder_signal(param);
}

void GuiKernel::confirmRereleaseTransponderPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<StringDictionary> param(new StringDictionary());
  param->insert("login", AbstractGui->LoginLineEdit2->text());
  param->insert("password", AbstractGui->PasswordLineEdit2->text());
  param->insert("pan", AbstractGui->RereleaseKeyLineEdit->text());
  param->insert("ucid", AbstractGui->UcidLineEdit->text());

  emit confirmTransponderRerelease_signal(param);
}

void GuiKernel::productionLineRollbackPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<StringDictionary> param(new StringDictionary());
  param->insert("login", AbstractGui->LoginLineEdit2->text());
  param->insert("password", AbstractGui->PasswordLineEdit2->text());

  emit rollbackProductionLine_signal(param);
}

void GuiKernel::printBoxStickerOnServerPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  PanInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit printBoxStickerOnServer_signal(param);
}

void GuiKernel::printLastBoxStickerOnServerPushButton_slot() {
  emit loggerClear_signal();

  emit printLastBoxStickerOnServer_signal();
}

void GuiKernel::printPalletStickerOnServerPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  PanInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit printPalletStickerOnServer_signal(param);
}

void GuiKernel::printLastPalletStickerOnServerPushButton_slot() {
  emit printLastPalletStickerOnServer_signal();
}

void GuiKernel::transponderManualReleasePushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  ManualReleaseRefundDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit releaseTranspondersManually_signal(param, TransponderModel);
}

void GuiKernel::transponderManualRefundPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  ManualReleaseRefundDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit refundTranspondersManually_signal(param, TransponderModel);
}

void GuiKernel::palletShipmentPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  PalletShippingDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit shipPallets_signal(param, TransponderModel);
}

void GuiKernel::printTransponderStickerPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit printTransponderSticker_signal(param, StickerModel);
}

void GuiKernel::printBoxStickerPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit printBoxSticker_signal(param, StickerModel);
}

void GuiKernel::printPalletStickerPushButton_slot() {
  std::shared_ptr<StringDictionary> param(new StringDictionary);

  StringInputDialog dialog(this);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

  emit loggerClear_signal();
  emit printPalletSticker_signal(param, StickerModel);
}

void GuiKernel::execStickerPrinterCommandScriptPushButton_slot() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  emit loggerClear_signal();

  std::shared_ptr<QStringList> commandScript(new QStringList(
      AbstractGui->StickerPrinterCommandScriptInput->toPlainText().split(
          "\n")));
  emit execPrinterStickerCommandScript_signal(commandScript);
}

void GuiKernel::settingsActionTrigger_slot() {
  emit loggerClear_signal();

  SettingsDialog dialog(this);
  connect(&dialog, &SettingsDialog::applyNewSettings, Interactor.get(),
          &InteractionSystem::applySettings);
  connect(&dialog, &SettingsDialog::applyNewSettings, Logger.get(),
          &LogSystem::applySettings);
  connect(&dialog, &SettingsDialog::applyNewSettings, Manager.get(),
          &AdminManager::applySettings);

  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
}

void GuiKernel::displayFirmware_slot(std::shared_ptr<QFile> firmware) {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  if (!firmware.get()->open(QIODevice::ReadOnly)) {
    Interactor->generateErrorMessage(
        "Не удалось открыть файл прошивки для отображения.");
  }
  AbstractGui->AssembledFirmwareView->setPlainText(
      firmware.get()->readAll().toHex());
  firmware.get()->close();
}

void GuiKernel::displayTransponderData_slot(
    std::shared_ptr<StringDictionary> transponderData) {
  TransponderData->setData(*transponderData.get());
  CurrentGUI->update();
}

/*
 * Приватные методы
 */

void GuiKernel::loadSettings() const {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(PROGRAM_NAME);

  QSettings::setDefaultFormat(QSettings::IniFormat);
}

// bool GuiKernel::checkReleaseTransponderInput() const {
//   MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);
//   QRegularExpression regex("[A-Fa-f0-9]+");
//   QString login = AbstractGui->LoginLineEdit2->text();
//   QString pass = AbstractGui->PasswordLineEdit2->text();
//   QString ucid = AbstractGui->UcidLineEdit->text();

//  if (ucid.size() != UCID_CHAR_LENGTH) {
//    return false;
//  }

//  QRegularExpressionMatch match = regex.match(ucid);
//  if ((!match.hasMatch()) || (match.captured(0) != ucid)) {
//    return false;
//  }

//  if ((login.size() == 0) || (login.size() > 20)) {
//    return false;
//  }

//  if ((pass.size() == 0) || (pass.size() > 20)) {
//    return false;
//  }

//  return true;
//}

// bool GuiKernel::checkConfirmRereleaseTransponderInput() const {
//   MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);
//   QRegularExpression ucidRegex("[A-Fa-f0-9]+");
//   QRegularExpression panRegex("[0-9]+");
//   QString choice = AbstractGui->RereleaseKeyComboBox->currentText();
//   QString input = AbstractGui->RereleaseKeyLineEdit->text();
//   QString ucid = AbstractGui->UcidLineEdit->text();
//   QString login = AbstractGui->LoginLineEdit2->text();
//   QString pass = AbstractGui->PasswordLineEdit2->text();

//  if ((login.size() == 0) || (login.size() > 20)) {
//    return false;
//  }

//  if ((pass.size() == 0) || (pass.size() > 20)) {
//    return false;
//  }

//  if (choice == "SN") {
//    if (input.toInt() == 0) {
//      return false;
//    }
//  } else if (choice == "PAN") {
//    if (input.length() != PAN_CHAR_LENGTH) {
//      return false;
//    }

//    QRegularExpressionMatch match = panRegex.match(input);
//    if ((!match.hasMatch()) || (match.captured(0) != input)) {
//      return false;
//    }
//  } else {
//    return false;
//  }

//  if (ucid.size() != UCID_CHAR_LENGTH) {
//    return false;
//  }

//  QRegularExpressionMatch match = ucidRegex.match(ucid);
//  if ((!match.hasMatch()) || (match.captured(0) != ucid)) {
//    return false;
//  }

//  return true;
//}

// bool GuiKernel::checkLinkIssuerInput() const {
//   MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);
//   QString issuerId = AbstractGui->IssuerIdLineEdit1->text();
//   QString masterKeysId = AbstractGui->MasterKeysLineEdit1->text();

//  if (issuerId.toInt() == 0) {
//    return false;
//  }

//  if (masterKeysId.toInt() == 0) {
//    return false;
//  }

//  return true;
//}

void GuiKernel::createTopMenu() {
  menuBar()->clear();
  createTopMenuActions();

  ServiceMenu = menuBar()->addMenu("Сервис");
  ServiceMenu->addAction(RequestAuthorizationGuiAction);
  ServiceMenu->addAction(SettingsAction);

  HelpMenu = menuBar()->addMenu("Справка");
  HelpMenu->addAction(AboutProgramAction);
}

void GuiKernel::createTopMenuActions() {
  SettingsAction = new QAction("Настройки");
  SettingsAction->setStatusTip("Открыть меню настроек");
  connect(SettingsAction, &QAction::triggered, this,
          &GuiKernel::settingsActionTrigger_slot);

  RequestAuthorizationGuiAction = new QAction("Авторизация");
  RequestAuthorizationGuiAction->setStatusTip(
      "Закрыть текущий интерфейс и создать начальный интерфейс");
  connect(RequestAuthorizationGuiAction, &QAction::triggered, this,
          &GuiKernel::requestAuthorizationGUIAct_slot);

  AboutProgramAction = new QAction("О программе", this);
  AboutProgramAction->setStatusTip("Показать сведения о программе");
}

void GuiKernel::createAuthorazationGUI() {
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
  connectAuthorizationGUI();
}

void GuiKernel::connectAuthorizationGUI() {
  AuthorizationGUI* AbstractGui = dynamic_cast<AuthorizationGUI*>(CurrentGUI);

  connect(AbstractGui->AuthorizePushButton, &QPushButton::clicked, this,
          &GuiKernel::authorizePushButton_slot);
}

void GuiKernel::createMainWindowGUI() {
  // Удаляем предыдущий интерфейс
  if (CurrentGUI) {
    CurrentGUI->hide();
    delete CurrentGUI;
  }
  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.7, DesktopGeometry.height() * 0.7);

  // Создаем интерфейс
  CurrentGUI = new MainWindowGUI(this);
  setCentralWidget(CurrentGUI);
  CurrentGUI->create();

  // Создаем верхнее меню
  createTopMenu();

  // Подключаем интерфейс
  connectMainWindowGUI();
}

void GuiKernel::connectMainWindowGUI() {
  MainWindowGUI* AbstractGui = dynamic_cast<MainWindowGUI*>(CurrentGUI);

  // База данных
  connect(AbstractGui->ConnectDatabasePushButton, &QPushButton::clicked, this,
          &GuiKernel::connectDatabasePushButton_slot);
  connect(AbstractGui->DisconnectDatabasePushButton, &QPushButton::clicked,
          this, &GuiKernel::disconnectDatabasePushButton_slot);
  connect(AbstractGui->ShowDatabaseTablePushButton, &QPushButton::clicked, this,
          &GuiKernel::showDatabaseTablePushButton_slot);
  connect(AbstractGui->InitIssuerTablePushButton, &QPushButton::clicked, this,
          &GuiKernel::initIssuerTablePushButton_slot);

  connect(AbstractGui->TransmitCustomRequestPushButton, &QPushButton::clicked,
          this, &GuiKernel::transmitCustomRequestPushButton_slot);

  // Заказы
  connect(AbstractGui->CreateNewOrderPushButton, &QPushButton::clicked, this,
          &GuiKernel::createNewOrderPushButton_slot);
  connect(AbstractGui->StartOrderAssemblingPushButton, &QPushButton::clicked,
          this, &GuiKernel::startOrderAssemblingPushButton_slot);
  connect(AbstractGui->StopOrderAssemblingPushButton, &QPushButton::clicked,
          this, &GuiKernel::stopOrderAssemblingPushButton_slot);
  connect(AbstractGui->UpdateOrderViewPushButton, &QPushButton::clicked, this,
          &GuiKernel::updateOrderViewPushButton_slot);

  // Производственные линии
  connect(AbstractGui->CreateNewProductionLinePushButton, &QPushButton::clicked,
          this, &GuiKernel::createNewProductionLinePushButton_slot);
  connect(AbstractGui->ActivateProductionLinePushButton, &QPushButton::clicked,
          this, &GuiKernel::activateProductionLinePushButton_slot);
  connect(AbstractGui->DeactivateProductionLinePushButton,
          &QPushButton::clicked, this,
          &GuiKernel::deactivateProductionLinePushButton_slot);
  connect(AbstractGui->ActivateAllProductionLinesPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::activateAllProductionLinesPushButton_slot);
  connect(AbstractGui->DeactivateAllProductionLinesPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::deactivateAllProductionLinesPushButton_slot);
  connect(AbstractGui->UpdateProductionLineViewPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::updateProductionLineViewPushButton_slot);

  // Эмитенты
  connect(AbstractGui->InitTransportMasterKeysPushButton, &QPushButton::clicked,
          this, &GuiKernel::initTransportMasterKeysPushButton_slot);
  connect(AbstractGui->LinkIssuerWithKeysPushButton, &QPushButton::clicked,
          this, &GuiKernel::linkIssuerWithKeysPushButton_slot);

  // Тест сервера
  connect(AbstractGui->ReleaseTransponderPushButton, &QPushButton::clicked,
          this, &GuiKernel::releaseTransponderPushButton_slot);
  connect(AbstractGui->ConfirmTransponderPushButton, &QPushButton::clicked,
          this, &GuiKernel::confirmTransponderPushButton_slot);
  connect(AbstractGui->RereleaseTransponderPushButton, &QPushButton::clicked,
          this, &GuiKernel::rereleaseTransponderPushButton_slot);
  connect(AbstractGui->ConfirmRereleaseTransponderPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::confirmRereleaseTransponderPushButton_slot);
  connect(AbstractGui->ProductionLineRollbackPushButton, &QPushButton::clicked,
          this, &GuiKernel::productionLineRollbackPushButton_slot);

  connect(AbstractGui->PrintBoxStickerOnServerPushButton, &QPushButton::clicked,
          this, &GuiKernel::printBoxStickerOnServerPushButton_slot);
  connect(AbstractGui->PrintLastBoxStickerOnServerPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::printLastBoxStickerOnServerPushButton_slot);
  connect(AbstractGui->PrintPalletStickerOnServerPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::printPalletStickerOnServerPushButton_slot);
  connect(AbstractGui->PrintLastPalletStickerOnServerPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::printLastPalletStickerOnServerPushButton_slot);

  // Транспондеры
  connect(AbstractGui->TransponderManualReleasePushButton,
          &QPushButton::clicked, this,
          &GuiKernel::transponderManualReleasePushButton_slot);
  connect(AbstractGui->TransponderManualRefundPushButton, &QPushButton::clicked,
          this, &GuiKernel::transponderManualRefundPushButton_slot);
  connect(AbstractGui->PalletShipmentPushButton, &QPushButton::clicked, this,
          &GuiKernel::palletShipmentPushButton_slot);

  // Стикеры
  connect(AbstractGui->PrintTransponderStickerPushButton, &QPushButton::clicked,
          this, &GuiKernel::printTransponderStickerPushButton_slot);
  connect(AbstractGui->PrintBoxStickerPushButton, &QPushButton::clicked, this,
          &GuiKernel::printBoxStickerPushButton_slot);
  connect(AbstractGui->PrintPalletStickerPushButton, &QPushButton::clicked,
          this, &GuiKernel::printPalletStickerPushButton_slot);
  connect(AbstractGui->ExecStickerPrinterCommandScriptPushButton,
          &QPushButton::clicked, this,
          &GuiKernel::execStickerPrinterCommandScriptPushButton_slot);

  // Соединяем модели и представления
  AbstractGui->DatabaseRandomModelView->setModel(RandomModel);
  AbstractGui->OrderTableView->setModel(OrderModel);
  AbstractGui->ProductionLineTableView->setModel(ProductionLineModel);
  AbstractGui->IssuerTableView->setModel(IssuerModel);
  AbstractGui->StickerDataTableView->setModel(StickerModel);
  AbstractGui->TransponderTableView->setModel(TransponderModel);

  AbstractGui->TransponderDataTableView->setModel(TransponderData);

  // Связываем отображения графиков с логикой их формирования
}

void GuiKernel::createLoggerInstance() {
  Logger = std::unique_ptr<LogSystem>(new LogSystem("LogSystem"));
  connect(this, &GuiKernel::loggerClear_signal, Logger.get(),
          &LogSystem::clear);
  connect(this, &GuiKernel::logging, Logger.get(), &LogSystem::generate);

  LoggerThread = std::unique_ptr<QThread>(new QThread());
  connect(LoggerThread.get(), &QThread::started, Logger.get(),
          &LogSystem::instanceThreadStarted);

  Logger->moveToThread(LoggerThread.get());
  LoggerThread->start();
}

void GuiKernel::createManagerInstance() {
  Manager = std::unique_ptr<AdminManager>(new AdminManager("AdminManager"));
  connect(Manager.get(), &AdminManager::executionStarted, Interactor.get(),
          &InteractionSystem::processOperationStart);
  connect(Manager.get(), &AdminManager::executionFinished, Interactor.get(),
          &InteractionSystem::processOperationFinish);

  // Подключаем функционал
  connect(this, &GuiKernel::connectDatabase_signal, Manager.get(),
          &AdminManager::connectDatabase);
  connect(this, &GuiKernel::disconnectDatabase_signal, Manager.get(),
          &AdminManager::disconnectDatabase);
  connect(this, &GuiKernel::showDatabaseTable_signal, Manager.get(),
          &AdminManager::showDatabaseTable);
  connect(this, &GuiKernel::performCustomRequest_signal, Manager.get(),
          &AdminManager::performCustomRequest);

  connect(this, &GuiKernel::createNewOrder_signal, Manager.get(),
          &AdminManager::createNewOrder);
  connect(this, &GuiKernel::startOrderAssembling_signal, Manager.get(),
          &AdminManager::startOrderAssembling);
  connect(this, &GuiKernel::stopOrderAssembling_signal, Manager.get(),
          &AdminManager::stopOrderAssembling);
  connect(this, &GuiKernel::showOrderTable_signal, Manager.get(),
          &AdminManager::showOrderTable);

  connect(this, &GuiKernel::createNewProductionLine_signal, Manager.get(),
          &AdminManager::createNewProductionLine);
  connect(this, &GuiKernel::activateProductionLine_signal, Manager.get(),
          &AdminManager::activateProductionLine);
  connect(this, &GuiKernel::activateAllProductionLines_signal, Manager.get(),
          &AdminManager::activateAllProductionLines);
  connect(this, &GuiKernel::deactivateProductionLine_signal, Manager.get(),
          &AdminManager::deactivateProductionLine);
  connect(this, &GuiKernel::deactivateAllProductionLines_signal, Manager.get(),
          &AdminManager::deactivateAllProductionLines);
  connect(this, &GuiKernel::showProductionLineTable_signal, Manager.get(),
          &AdminManager::showProductionLineTable);
  connect(this, &GuiKernel::showProductionLineTable_signal, Manager.get(),
          &AdminManager::showProductionLineTable);

  connect(this, &GuiKernel::releaseTransponder_signal, Manager.get(),
          &AdminManager::releaseTransponder);
  connect(this, &GuiKernel::confirmTransponderRelease_signal, Manager.get(),
          &AdminManager::confirmTransponderRelease);
  connect(this, &GuiKernel::rereleaseTransponder_signal, Manager.get(),
          &AdminManager::rereleaseTransponder);
  connect(this, &GuiKernel::confirmTransponderRerelease_signal, Manager.get(),
          &AdminManager::confirmTransponderRerelease);
  connect(this, &GuiKernel::rollbackProductionLine_signal, Manager.get(),
          &AdminManager::rollbackProductionLine);
  connect(this, &GuiKernel::printBoxStickerOnServer_signal, Manager.get(),
          &AdminManager::printBoxStickerOnServer);
  connect(this, &GuiKernel::printLastBoxStickerOnServer_signal, Manager.get(),
          &AdminManager::printLastBoxStickerOnServer);
  connect(this, &GuiKernel::printPalletStickerOnServer_signal, Manager.get(),
          &AdminManager::printPalletStickerOnServer);
  connect(this, &GuiKernel::printLastPalletStickerOnServer_signal,
          Manager.get(), &AdminManager::printLastPalletStickerOnServer);

  connect(this, &GuiKernel::releaseTranspondersManually_signal, Manager.get(),
          &AdminManager::releaseTranspondersManually);
  connect(this, &GuiKernel::refundTranspondersManually_signal, Manager.get(),
          &AdminManager::refundTranspondersManually);
  connect(this, &GuiKernel::shipPallets_signal, Manager.get(),
          &AdminManager::shipPallets);

  connect(this, &GuiKernel::initIssuers_signal, Manager.get(),
          &AdminManager::initIssuers);
  connect(this, &GuiKernel::initTransportMasterKeys_signal, Manager.get(),
          &AdminManager::initTransportMasterKeys);
  connect(this, &GuiKernel::linkIssuerWithMasterKeys_signal, Manager.get(),
          &AdminManager::linkIssuerWithMasterKeys);

  connect(this, &GuiKernel::printTransponderSticker_signal, Manager.get(),
          &AdminManager::printTransponderSticker);
  connect(this, &GuiKernel::printBoxSticker_signal, Manager.get(),
          &AdminManager::printBoxSticker);
  connect(this, &GuiKernel::printPalletSticker_signal, Manager.get(),
          &AdminManager::printPalletSticker);
  connect(this, &GuiKernel::execPrinterStickerCommandScript_signal,
          Manager.get(), &AdminManager::execPrinterStickerCommandScript);

  // Запросы на отображение
  connect(Manager.get(), &AdminManager::displayFirmware_signal, this,
          &GuiKernel::displayFirmware_slot);
  connect(Manager.get(), &AdminManager::displayTransponderData_signal, this,
          &GuiKernel::displayTransponderData_slot);

  // Поток
  ManagerThread = std::unique_ptr<QThread>(new QThread());
  connect(ManagerThread.get(), &QThread::started, Manager.get(),
          &AdminManager::insctanceThreadStarted_slot);

  Manager->moveToThread(ManagerThread.get());
  ManagerThread->start();
}

void GuiKernel::createInteractorInstance() {
  Interactor = std::unique_ptr<InteractionSystem>(
      new InteractionSystem("InteractionSystem"));
}

void GuiKernel::createModels() {
  RandomModel = new SqlQueryValues(this);
  OrderModel = new SqlQueryValues(this);
  ProductionLineModel = new SqlQueryValues(this);
  IssuerModel = new SqlQueryValues(this);
  StickerModel = new SqlQueryValues(this);
  TransponderModel = new SqlQueryValues(this);

  TransponderData = new HashTableModel(this);
}

void GuiKernel::createMatchingTable() {
  MatchingTable.insert("Транспортные мастер ключи", "transport_master_keys");
  MatchingTable.insert("Коммерческие мастер ключи", "commercial_master_keys");
  MatchingTable.insert("Эмитенты", "issuers");
}

void GuiKernel::registerMetaType() {
  qRegisterMetaType<std::shared_ptr<QMap<QString, QString>>>(
      "std::shared_ptr<StringDictionary >");
  qRegisterMetaType<std::shared_ptr<StringDictionary>>(
      "std::shared_ptr<StringDictionary >");
  qRegisterMetaType<std::shared_ptr<QStringList>>(
      "std::shared_ptr<QStringList>");
  qRegisterMetaType<ReturnStatus>("ReturnStatus");
  qRegisterMetaType<std::shared_ptr<QFile>>("std::shared_ptr<QFile>");
}
