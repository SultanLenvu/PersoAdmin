#include "admin_manager.h"
#include "Log/log_system.h"
#include "StickerPrinter/te310_printer.h"

AdminManager::AdminManager(const QString& name) : QObject(nullptr) {
  setObjectName(name);
  loadSettings();
}

AdminManager::~AdminManager() {}

void AdminManager::insctanceThreadStarted_slot() {
  // Создаем администратора
  createAdministrator();

  // Создаем клиента
  createClient();

  // Создаем стикер принтер
  createStickerPrinter();
}

void AdminManager::connectDatabase() {
  // Начинаем выполнение операции
  emit executionStarted("connectDatabase");

  ReturnStatus status;

  sendLog("Подключение к базе данных. ");
  status = Administrator->connectDatabase();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "connectDatabase");
    return;
  }

  emit executionFinished("connectDatabase", ReturnStatus::NoError);
}

void AdminManager::disconnectDatabase() {
  // Начинаем выполнение операции
  emit executionStarted("disconnectDatabase");

  ReturnStatus status;

  sendLog("Отключение от базы данных. ");
  status = Administrator->disconnectDatabase();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "disconnectDatabase");
    return;
  }

  emit executionFinished("disconnectDatabase", ReturnStatus::NoError);
}

void AdminManager::showDatabaseTable(const QString& name,
                                     SqlQueryValues* model) {
  // Начинаем выполнение операции
  emit executionStarted("showDatabaseTable");

  ReturnStatus status;

  model->clear();
  sendLog(QString("Отображение таблицы %1. ").arg(name));
  status = Administrator->getTable(name, model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "showDatabaseTable");
    return;
  }

  emit executionFinished("showDatabaseTable", ReturnStatus::NoError);
}

void AdminManager::performCustomRequest(const QString& req,
                                        SqlQueryValues* model) {
  // Начинаем выполнение операции
  emit executionStarted("performCustomRequest");

  ReturnStatus status;

  model->clear();
  sendLog("Представление ответа на кастомный запрос. ");
  status = Administrator->getCustomResponse(req, model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "performCustomRequest");
    return;
  }

  emit executionFinished("performCustomRequest", ReturnStatus::NoError);
}

void AdminManager::createNewOrder(
    const std::shared_ptr<QHash<QString, QString>> orderParameters,
    SqlQueryValues* model) {
  emit executionStarted("createNewOrder");

  ReturnStatus status;

  sendLog("Создание нового заказа. ");
  status = Administrator->createNewOrder(orderParameters);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "createNewOrder");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getTable("orders", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "createNewOrder");
    return;
  }

  emit executionFinished("createNewOrder", ReturnStatus::NoError);
}

void AdminManager::startOrderAssembling(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("startOrderAssembling");

  ReturnStatus status;

  sendLog(QString("Запуск сборки заказа %1. ").arg(param->value("id")));
  status = Administrator->startOrderAssembling(param->value("id"));
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "startOrderAssembling");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getTable("orders", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "startOrderAssembling");
    return;
  }

  emit executionFinished("startOrderAssembling", ReturnStatus::NoError);
}

void AdminManager::stopOrderAssembling(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("stopOrderAssembling");

  ReturnStatus status;

  sendLog(QString("Остановка сборки заказа %1. ").arg(param->value("id")));
  status = Administrator->stopOrderAssembling(param->value("id"));
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "stopOrderAssembling");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getTable("orders", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "stopOrderAssembling");
    return;
  }

  emit executionFinished("stopOrderAssembling", ReturnStatus::NoError);
}

void AdminManager::showOrderTable(SqlQueryValues* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::createNewProductionLine(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("createNewProductionLine");

  ReturnStatus status;

  sendLog("Создание новой линии производства. ");
  status = Administrator->createNewProductionLine(param.get());
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "createNewProductionLine");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getTable("production_lines", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "createNewProductionLine");
    return;
  }

  emit executionFinished("createNewProductionLine", ReturnStatus::NoError);
}

void AdminManager::activateProductionLine(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("activateProductionLine");

  ReturnStatus status;

  sendLog("Запуск линии производства. ");
  status =
      Administrator->activateProductionLine(param->value("production_line_id"));
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "activateProductionLine");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getTable("production_lines", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "activateProductionLine");
    return;
  }

  emit executionFinished("activateProductionLine", ReturnStatus::NoError);
}

void AdminManager::activateAllProductionLines(SqlQueryValues* model) {
  emit executionStarted("activateAllProductionLines");

  ReturnStatus status;

  sendLog(QString("Остановка всех производственных линий. "));
  status = Administrator->activateAllProductionLines();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "activateAllProductionLines");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getTable("production_lines", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "activateAllProductionLines");
    return;
  }

  emit executionFinished("activateAllProductionLines", ReturnStatus::NoError);
}

void AdminManager::deactivateProductionLine(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("deactivateProductionLine");

  ReturnStatus status;

  sendLog("Остановка линии производства. ");
  status = Administrator->deactivateProductionLine(param->value("id"));
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "deactivateProductionLine");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getTable("production_lines", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "deactivateProductionLine");
    return;
  }

  emit executionFinished("deactivateProductionLine", ReturnStatus::NoError);
}

void AdminManager::deactivateAllProductionLines(SqlQueryValues* model) {
  emit executionStarted("deactivateAllProductionLines");

  ReturnStatus status;

  sendLog(QString("Остановка всех производственных линий. "));
  status = Administrator->deactivateAllProductionLines();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "deactivateAllProductionLines");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getTable("production_lines", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "deactivateAllProductionLines");
    return;
  }

  emit executionFinished("deactivateAllProductionLines", ReturnStatus::NoError);
}

void AdminManager::showProductionLineTable(SqlQueryValues* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::initIssuers(SqlQueryValues* model) {
  emit executionStarted("initIssuers");

  ReturnStatus status;

  sendLog("Инициализация данных об эмитентах. ");
  status = Administrator->initIssuerTable();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "initIssuers");
    return;
  }

  model->clear();
  sendLog("Отображение эмитентов. ");
  status = Administrator->getTable("issuers", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "initIssuers");
    return;
  }

  emit executionFinished("initIssuers", ReturnStatus::NoError);
}

void AdminManager::initTransportMasterKeys(SqlQueryValues* model) {
  emit executionStarted("initTransportMasterKeys");

  ReturnStatus status;

  sendLog("Инициализация транспортных мастер ключей. ");
  status = Administrator->initTransportMasterKeysTable();
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "initTransportMasterKeys");
    return;
  }

  model->clear();
  sendLog("Отображение транспортных мастер ключей. ");
  status = Administrator->getTable("transport_master_keys", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "initTransportMasterKeys");
    return;
  }

  emit executionFinished("initTransportMasterKeys", ReturnStatus::NoError);
}

void AdminManager::linkIssuerWithMasterKeys(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("linkIssuerWithMasterKeys");

  ReturnStatus status;

  sendLog(QString("Связывание эмитента %1 с мастер ключами %2. ")
              .arg(param->value("issuer_id"), param->value("key_group_id")));
  status = Administrator->linkIssuerWithMasterKeys(param.get());
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "linkIssuerWithMasterKeys");
    return;
  }

  model->clear();
  sendLog("Отображение таблицы эмитентов. ");
  status = Administrator->getTable("issuers", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "linkIssuerWithMasterKeys");
    return;
  }

  emit executionFinished("linkIssuerWithMasterKeys", ReturnStatus::NoError);
}

void AdminManager::releaseTranspondersManually(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("releaseTranspondersManually");
  sendLog("Принудительный выпуск транспондеров. ");

  ReturnStatus status;
  QString table = param->value("table");

  if (table == "transponders") {
    status = Administrator->releaseTransponderManually(param->value("id"));
  } else if (table == "boxes") {
    status = Administrator->releaseBoxManually(param->value("id"));
  } else if (table == "pallets") {
    status = Administrator->releasePalletManually(param->value("id"));
  } else if (table == "orders") {
    status = Administrator->releaseOrderManually(param->value("id"));
  } else {
    processAdministratorError(ReturnStatus::ParameterError,
                              "releaseTranspondersManually");
    return;
  }

  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "releaseTranspondersManually");
    return;
  }

  status = Administrator->getTable(param->value("table"), model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "releaseTranspondersManually");
    return;
  }

  emit executionFinished("releaseTranspondersManually", ReturnStatus::NoError);
}

void AdminManager::refundTranspondersManually(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("refundTranspondersManually");
  sendLog("Возврат транспондеров. ");

  ReturnStatus status;
  QString table = param->value("table");

  if (table == "transponders") {
    status = Administrator->refundTransponderManually(param->value("id"));
  } else if (table == "boxes") {
    status = Administrator->refundBoxManually(param->value("id"));
  } else if (table == "pallets") {
    status = Administrator->refundPalletManually(param->value("id"));
  } else if (table == "orders") {
    status = Administrator->refundOrderManually(param->value("id"));
  } else {
    processAdministratorError(ReturnStatus::ParameterError,
                              "refundTranspondersManually");
    return;
  }

  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "refundTranspondersManually");
    return;
  }

  status = Administrator->getTable(param->value("table"), model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "refundTranspondersManually");
    return;
  }

  emit executionFinished("refundTranspondersManually", ReturnStatus::NoError);
}

void AdminManager::shipPallets(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("shipPallets");
  sendLog("Отгрузка паллет. ");

  ReturnStatus status;
  status = Administrator->shipPallets(param.get());
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "shipPallets");
    return;
  }

  status = Administrator->getTable("pallets", model);
  if (status != ReturnStatus::NoError) {
    processAdministratorError(status, "shipPallets");
    return;
  }

  emit executionFinished("shipPallets", ReturnStatus::NoError);
}

void AdminManager::releaseTransponder(
    const std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("releaseTransponder");
  sendLog("Выпуск транспондера. ");

  std::shared_ptr<QFile> firmware(new QFile("temp.bin"));
  std::shared_ptr<QHash<QString, QString>> transponderData(
      new QHash<QString, QString>());
  PersoClient::ReturnStatus status = Client->requestTransponderRelease(
      param.get(), firmware.get(), transponderData.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "releaseTransponder");
    return;
  }

  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  emit displayFirmware_signal(firmware);

  sendLog("Запрос на отображение данных транспондера. ");
  emit displayTransponderData_signal(transponderData);

  emit executionFinished("releaseTransponder", ReturnStatus::NoError);
}

void AdminManager::confirmTransponderRelease(
    const std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("confirmTransponderRelease");
  sendLog("Подтверждение выпуска транспондера. ");

  PersoClient::ReturnStatus status =
      Client->requestTransponderReleaseConfirm(param.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "confirmTransponderRelease");
    return;
  }

  emit executionFinished("confirmTransponderRelease", ReturnStatus::NoError);
}

void AdminManager::rereleaseTransponder(
    const std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("rereleaseTransponder");
  sendLog("Перевыпуск транспондера. ");

  std::shared_ptr<QFile> firmware(new QFile("temp.bin"));
  std::shared_ptr<QHash<QString, QString>> transponderData(
      new QHash<QString, QString>());
  PersoClient::ReturnStatus status = Client->requestTransponderRerelease(
      param.get(), firmware.get(), transponderData.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "releaseTransponder");
    return;
  }

  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  emit displayFirmware_signal(firmware);
  sendLog("Запрос на отображение данных транспондера. ");
  emit displayTransponderData_signal(transponderData);

  emit executionFinished("rereleaseTransponder", ReturnStatus::NoError);
}

void AdminManager::confirmTransponderRerelease(
    const std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("confirmTransponderRerelease");
  sendLog("Подтверждение перевыпуска транспондера. ");

  PersoClient::ReturnStatus status =
      Client->requestTransponderRereleaseConfirm(param.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "confirmTransponderRelease");
    return;
  }

  emit executionFinished("confirmTransponderRerelease", ReturnStatus::NoError);
}

void AdminManager::rollbackProductionLine(
    const std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("rollbackProductionLine");
  sendLog("Откат производственной линии. ");

  PersoClient::ReturnStatus status =
      Client->requestProductionLineRollback(param.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "rollbackProductionLine");
    return;
  }

  emit executionFinished("rollbackProductionLine", ReturnStatus::NoError);
}

void AdminManager::printBoxStickerOnServer(
    std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("printBoxStickerOnServer");
  sendLog("Печать стикера для бокса на сервере. ");

  PersoClient::ReturnStatus status =
      Client->requestBoxStickerPrint(param.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "printBoxStickerOnServer");
    return;
  }

  emit executionFinished("printBoxStickerOnServer");
}

void AdminManager::printLastBoxStickerOnServer() {
  emit executionStarted("printLastBoxStickerOnServer");
  sendLog("Повторная печать последнего стикера для бокса на сервере. ");

  PersoClient::ReturnStatus status = Client->requestBoxStickerReprint();
  if (status != PersoClient::NoError) {
    processClientError(status, "printLastBoxStickerOnServer");
    return;
  }

  emit executionFinished("printLastBoxStickerOnServer", ReturnStatus::NoError);
}

void AdminManager::printPalletStickerOnServer(
    std::shared_ptr<QHash<QString, QString>> param) {
  emit executionStarted("printPalletStickerOnServer");
  sendLog("Печать стикера для паллеты на сервере. ");

  PersoClient::ReturnStatus status =
      Client->requestPalletStickerPrint(param.get());
  if (status != PersoClient::NoError) {
    processClientError(status, "printPalletStickerOnServer");
    return;
  }

  emit executionFinished("printPalletStickerOnServer", ReturnStatus::NoError);
}

void AdminManager::printLastPalletStickerOnServer() {
  emit executionStarted("printLastPalletStickerOnServer");
  sendLog("Повторная печать последнего стикера для паллеты на сервере. ");

  PersoClient::ReturnStatus status = Client->requestPalletStickerReprint();
  if (status != PersoClient::NoError) {
    processClientError(status, "printLastPalletStickerOnServer");
    return;
  }

  emit executionFinished("printLastPalletStickerOnServer",
                         ReturnStatus::NoError);
}

void AdminManager::printTransponderSticker(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("printTransponderSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  ReturnStatus administratorStatus;

  QHash<QString, QString> transponderData;
  sendLog("Запрос данных транспондера. ");
  administratorStatus =
      Administrator->getTransponderData(param->value("id"), &transponderData);
  if (administratorStatus != ReturnStatus::NoError) {
    processAdministratorError(administratorStatus, "printTransponderSticker");
    return;
  }

  sendLog("Печать стикера транспондера. ");
  stickerPrinterStatus =
      StickerPrinter->printTransponderSticker(&transponderData);
  if (stickerPrinterStatus != IStickerPrinter::NoError) {
    processStickerPrinterError(stickerPrinterStatus, "printTransponderSticker");
    return;
  }

  emit executionFinished("printTransponderSticker", ReturnStatus::NoError);
}

void AdminManager::printBoxSticker(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("printBoxSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  ReturnStatus administratorStatus;

  QHash<QString, QString> boxData;
  sendLog("Запрос данных бокса. ");
  administratorStatus = Administrator->getBoxData(param->value("id"), &boxData);
  if (administratorStatus != ReturnStatus::NoError) {
    processAdministratorError(administratorStatus, "printBoxSticker");
    return;
  }

  sendLog("Печать стикера бокса. ");
  stickerPrinterStatus = StickerPrinter->printBoxSticker(&boxData);
  if (stickerPrinterStatus != IStickerPrinter::NoError) {
    processStickerPrinterError(stickerPrinterStatus, "printBoxSticker");
    return;
  }

  emit executionFinished("printBoxSticker", ReturnStatus::NoError);
}

void AdminManager::printPalletSticker(
    const std::shared_ptr<QHash<QString, QString>> param,
    SqlQueryValues* model) {
  emit executionStarted("printPalletSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  ReturnStatus administratorStatus;

  QHash<QString, QString> palletData;
  sendLog("Запрос данных паллеты. ");
  administratorStatus =
      Administrator->getPalletData(param->value("id"), &palletData);
  if (administratorStatus != ReturnStatus::NoError) {
    processAdministratorError(administratorStatus, "printTransponderSticker");
    return;
  }

  sendLog("Печать стикера паллеты. ");
  stickerPrinterStatus = StickerPrinter->printPalletSticker(&palletData);
  if (stickerPrinterStatus != IStickerPrinter::NoError) {
    processStickerPrinterError(stickerPrinterStatus, "printPalletSticker");
    return;
  }

  emit executionFinished("printPalletSticker", ReturnStatus::NoError);
}

void AdminManager::execPrinterStickerCommandScript(
    const std::shared_ptr<QStringList> commandScript) {
  emit executionStarted("execPrinterStickerCommandScript");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;

  sendLog("Выполнение командного скрипта принтера стикеров. ");
  stickerPrinterStatus = StickerPrinter->exec(commandScript.get());
  if (stickerPrinterStatus != IStickerPrinter::NoError) {
    processStickerPrinterError(stickerPrinterStatus,
                               "execPrinterStickerCommandScript");
    return;
  }

  emit executionFinished("execPrinterStickerCommandScript",
                         ReturnStatus::NoError);
}

void AdminManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Administrator->applySettings();
  //  Client->applySettings();
  StickerPrinter->applySetting();
}

void AdminManager::loadSettings() {
  QSettings settings;
}

void AdminManager::sendLog(const QString& log) {
  emit logging(QString("%1 - %2").arg(objectName(), log));
}

void AdminManager::createAdministrator() {
  Administrator = std::unique_ptr<AdministrationSystem>(
      new AdministrationSystem("AdministrationSystem"));
  connect(Administrator.get(), &AdministrationSystem::logging,
          LogSystem::instance(), &LogSystem::generate);
}

void AdminManager::createStickerPrinter() {
  StickerPrinter =
      std::unique_ptr<IStickerPrinter>(new TE310Printer(this, "RandomPrinter"));
  connect(StickerPrinter.get(), &IStickerPrinter::logging,
          LogSystem::instance(), &LogSystem::generate);
}


//==================================================================================
