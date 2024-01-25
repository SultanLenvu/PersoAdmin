#include "admin_manager.h"
#include "global_environment.h"
#include "log_system.h"
#include "te310_printer.h"

AdminManager::AdminManager(const QString& name) : QObject(nullptr) {
  setObjectName(name);
  loadSettings();

  connect(this, &AdminManager::logging,
          dynamic_cast<LogSystem*>(
              GlobalEnvironment::instance()->getObject("LogSystem")),
          &LogSystem::generate);

  GlobalEnvironment::instance()->registerObject(this);
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

  ReturnStatus ret;

  sendLog("Подключение к базе данных. ");
  ret = Administrator->connectDatabase();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("connectDatabase", ret);
    return;
  }

  emit executionFinished("connectDatabase", ReturnStatus::NoError);
}

void AdminManager::disconnectDatabase() {
  // Начинаем выполнение операции
  emit executionStarted("disconnectDatabase");

  ReturnStatus ret;

  sendLog("Отключение от базы данных. ");
  ret = Administrator->disconnectDatabase();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("disconnectDatabase", ret);
    return;
  }

  emit executionFinished("disconnectDatabase", ReturnStatus::NoError);
}

void AdminManager::showDatabaseTable(const QString& name,
                                     SqlQueryValues* model) {
  // Начинаем выполнение операции
  emit executionStarted("showDatabaseTable");

  ReturnStatus ret;

  model->clear();
  sendLog(QString("Отображение таблицы %1. ").arg(name));
  ret = Administrator->getTable(name, *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("showDatabaseTable", ret);
    return;
  }

  emit executionFinished("showDatabaseTable", ReturnStatus::NoError);
}

void AdminManager::performCustomRequest(const QString& req,
                                        SqlQueryValues* model) {
  // Начинаем выполнение операции
  emit executionStarted("performCustomRequest");

  ReturnStatus ret;

  model->clear();
  sendLog("Представление ответа на кастомный запрос. ");
  ret = Administrator->getCustomResponse(req, *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("performCustomRequest", ret);
    return;
  }

  emit executionFinished("performCustomRequest", ReturnStatus::NoError);
}

void AdminManager::createNewOrder(const std::shared_ptr<StringDictionary> param,
                                  SqlQueryValues* model) {
  emit executionStarted("createNewOrder");

  ReturnStatus ret;

  sendLog("Создание нового заказа. ");
  ret = Administrator->createNewOrder(*param);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("createNewOrder", ret);
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  ret = Administrator->getTable("orders", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("createNewOrder", ret);
    return;
  }

  emit executionFinished("createNewOrder", ReturnStatus::NoError);
}

void AdminManager::startOrderAssembling(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("startOrderAssembling");

  ReturnStatus ret;

  sendLog(QString("Запуск сборки заказа %1. ").arg(param->value("id")));
  ret = Administrator->startOrderAssembling(param->value("id"));
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("startOrderAssembling", ret);
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  ret = Administrator->getTable("orders", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("startOrderAssembling", ret);
    return;
  }

  emit executionFinished("startOrderAssembling", ReturnStatus::NoError);
}

void AdminManager::stopOrderAssembling(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("stopOrderAssembling");

  ReturnStatus ret;

  ret = Administrator->stopOrderAssembling(param->value("id"));
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("stopOrderAssembling", ret);
    return;
  }
  sendLog(QString("Сборка заказа %1 остановлена. ").arg(param->value("id")));

  model->clear();
  sendLog("Отображение заказов. ");
  ret = Administrator->getTable("orders", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("stopOrderAssembling", ret);
    return;
  }

  emit executionFinished("stopOrderAssembling", ReturnStatus::NoError);
}

void AdminManager::showOrderTable(SqlQueryValues* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::createNewProductionLine(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("createNewProductionLine");

  ReturnStatus ret;

  sendLog("Создание новой линии производства. ");
  ret = Administrator->createNewProductionLine(*param.get());
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("createNewProductionLine", ret);
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  ret = Administrator->getTable("production_lines", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("createNewProductionLine", ret);
    return;
  }

  emit executionFinished("createNewProductionLine", ReturnStatus::NoError);
}

void AdminManager::activateProductionLine(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("activateProductionLine");

  ReturnStatus ret;

  sendLog("Запуск линии производства. ");
  ret = Administrator->activateProductionLine(param->value("id"));
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("activateProductionLine", ret);
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  ret = Administrator->getTable("production_lines", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("activateProductionLine", ret);
    return;
  }

  emit executionFinished("activateProductionLine", ReturnStatus::NoError);
}

void AdminManager::activateAllProductionLines(SqlQueryValues* model) {
  emit executionStarted("activateAllProductionLines");

  ReturnStatus ret;

  sendLog(QString("Остановка всех производственных линий. "));
  ret = Administrator->activateAllProductionLines();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("activateAllProductionLines", ret);
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  ret = Administrator->getTable("production_lines", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("activateAllProductionLines", ret);
    return;
  }

  emit executionFinished("activateAllProductionLines", ReturnStatus::NoError);
}

void AdminManager::deactivateProductionLine(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("deactivateProductionLine");

  ReturnStatus ret;

  sendLog("Остановка линии производства. ");
  ret = Administrator->deactivateProductionLine(param->value("id"));
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("deactivateProductionLine", ret);
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  ret = Administrator->getTable("production_lines", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("deactivateProductionLine", ret);
    return;
  }

  emit executionFinished("deactivateProductionLine", ReturnStatus::NoError);
}

void AdminManager::deactivateAllProductionLines(SqlQueryValues* model) {
  emit executionStarted("deactivateAllProductionLines");

  ReturnStatus ret;

  sendLog(QString("Остановка всех производственных линий. "));
  ret = Administrator->deactivateAllProductionLines();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("deactivateAllProductionLines", ret);
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  ret = Administrator->getTable("production_lines", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("deactivateAllProductionLines", ret);
    return;
  }

  emit executionFinished("deactivateAllProductionLines", ReturnStatus::NoError);
}

void AdminManager::showProductionLineTable(SqlQueryValues* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::initIssuers(SqlQueryValues* model) {
  emit executionStarted("initIssuers");

  ReturnStatus ret;

  sendLog("Инициализация данных об эмитентах. ");
  ret = Administrator->initIssuerTable();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("initIssuers", ret);
    return;
  }

  model->clear();
  sendLog("Отображение эмитентов. ");
  ret = Administrator->getTable("issuers", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("initIssuers", ret);
    return;
  }

  emit executionFinished("initIssuers", ReturnStatus::NoError);
}

void AdminManager::initTransportMasterKeys(SqlQueryValues* model) {
  emit executionStarted("initTransportMasterKeys");

  ReturnStatus ret;

  sendLog("Инициализация транспортных мастер ключей. ");
  ret = Administrator->initTransportMasterKeysTable();
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("initTransportMasterKeys", ret);
    return;
  }

  model->clear();
  sendLog("Отображение транспортных мастер ключей. ");
  ret = Administrator->getTable("transport_master_keys", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("initTransportMasterKeys", ret);
    return;
  }

  emit executionFinished("initTransportMasterKeys", ReturnStatus::NoError);
}

void AdminManager::linkIssuerWithMasterKeys(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("linkIssuerWithMasterKeys");

  ReturnStatus ret;

  sendLog(QString("Связывание эмитента %1 с мастер ключами %2. ")
              .arg(param->value("issuer_id"), param->value("key_group_id")));
  ret = Administrator->linkIssuerWithMasterKeys(*param.get());
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("linkIssuerWithMasterKeys", ret);
    return;
  }

  model->clear();
  sendLog("Отображение таблицы эмитентов. ");
  ret = Administrator->getTable("issuers", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("linkIssuerWithMasterKeys", ret);
    return;
  }

  emit executionFinished("linkIssuerWithMasterKeys", ReturnStatus::NoError);
}

void AdminManager::releaseTranspondersManually(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("releaseTranspondersManually");
  sendLog("Принудительный выпуск транспондеров. ");

  ReturnStatus ret;
  QString table = param->value("table");

  if (table == "transponders") {
    ret = Administrator->releaseTransponder(param->value("id"));
  } else if (table == "boxes") {
    ret = Administrator->releaseBox(param->value("id"));
  } else if (table == "pallets") {
    ret = Administrator->releasePallet(param->value("id"));
  } else if (table == "orders") {
    ret = Administrator->releaseOrder(param->value("id"));
  } else {
    emit executionFinished("releaseTranspondersManually",
                           ReturnStatus::ParameterError);
    return;
  }

  if (ret != ReturnStatus::NoError) {
    emit executionFinished("releaseTranspondersManually", ret);
    return;
  }

  ret = Administrator->getTable(param->value("table"), *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("releaseTranspondersManually", ret);
    return;
  }

  emit executionFinished("releaseTranspondersManually", ReturnStatus::NoError);
}

void AdminManager::refundTranspondersManually(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("refundTranspondersManually");
  sendLog("Возврат транспондеров. ");

  ReturnStatus ret;
  QString table = param->value("table");

  if (table == "transponders") {
    ret = Administrator->refundTransponder(param->value("id"));
  } else if (table == "boxes") {
    ret = Administrator->refundBox(param->value("id"));
  } else if (table == "pallets") {
    ret = Administrator->refundPallet(param->value("id"));
  } else if (table == "orders") {
    ret = Administrator->refundOrder(param->value("id"));
  } else {
    emit executionFinished("refundTranspondersManually",
                           ReturnStatus::ParameterError);
    return;
  }

  if (ret != ReturnStatus::NoError) {
    emit executionFinished("refundTranspondersManually", ret);
    return;
  }

  ret = Administrator->getTable(param->value("table"), *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("refundTranspondersManually", ret);
    return;
  }

  emit executionFinished("refundTranspondersManually", ReturnStatus::NoError);
}

void AdminManager::shipPallets(const std::shared_ptr<StringDictionary> param,
                               SqlQueryValues* model) {
  emit executionStarted("shipPallets");
  sendLog("Отгрузка паллет. ");

  ReturnStatus ret;
  ret = Administrator->shipPallets(*param.get());
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("shipPallets", ret);
    return;
  }

  ret = Administrator->getTable("pallets", *model);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("shipPallets", ret);
    return;
  }

  emit executionFinished("shipPallets", ReturnStatus::NoError);
}

void AdminManager::releaseTransponder(
    const std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("releaseTransponder");
  //  sendLog("Выпуск транспондера. ");

  //  std::shared_ptr<QFile> firmware(new QFile("temp.bin"));
  //  std::shared_ptr<StringDictionary> transponderData(
  //      new StringDictionary());
  //  PersoClient::ReturnStatus ret = Client->requestTransponderRelease(
  //      *param.get(), firmware.get(), transponderData.get());
  //  if (ret != PersoClient::NoError) {
  //    emit executionFinished("releaseTransponder", ret);
  //    return;
  //  }

  //  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  //  emit displayFirmware_signal(firmware);

  //  sendLog("Запрос на отображение данных транспондера. ");
  //  emit displayTransponderData_signal(transponderData);

  //  emit executionFinished("releaseTransponder", ReturnStatus::NoError);
}

void AdminManager::confirmTransponderRelease(
    const std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("confirmTransponderRelease");
  //  sendLog("Подтверждение выпуска транспондера. ");

  //  PersoClient::ReturnStatus ret =
  //      Client->requestTransponderReleaseConfirm(*param.get());
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "confirmTransponderRelease");
  //    return;
  //  }

  //  emit executionFinished("confirmTransponderRelease",
  //  ReturnStatus::NoError);
}

void AdminManager::rereleaseTransponder(
    const std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("rereleaseTransponder");
  //  sendLog("Перевыпуск транспондера. ");

  //  std::shared_ptr<QFile> firmware(new QFile("temp.bin"));
  //  std::shared_ptr<StringDictionary> transponderData(
  //      new StringDictionary());
  //  PersoClient::ReturnStatus ret = Client->requestTransponderRerelease(
  //      *param.get(), firmware.get(), transponderData.get());
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "releaseTransponder");
  //    return;
  //  }

  //  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  //  emit displayFirmware_signal(firmware);
  //  sendLog("Запрос на отображение данных транспондера. ");
  //  emit displayTransponderData_signal(transponderData);

  //  emit executionFinished("rereleaseTransponder", ReturnStatus::NoError);
}

void AdminManager::confirmTransponderRerelease(
    const std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("confirmTransponderRerelease");
  //  sendLog("Подтверждение перевыпуска транспондера. ");

  //  PersoClient::ReturnStatus ret =
  //      Client->requestTransponderRereleaseConfirm(*param.get());
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "confirmTransponderRelease");
  //    return;
  //  }

  //  emit executionFinished("confirmTransponderRerelease",
  //  ReturnStatus::NoError);
}

void AdminManager::rollbackProductionLine(
    const std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("rollbackProductionLine");
  //  sendLog("Откат производственной линии. ");

  //  PersoClient::ReturnStatus ret =
  //      Client->requestProductionLineRollback(*param.get());
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "rollbackProductionLine");
  //    return;
  //  }

  //  emit executionFinished("rollbackProductionLine", ReturnStatus::NoError);
}

void AdminManager::printBoxStickerOnServer(
    std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("printBoxStickerOnServer");
  //  sendLog("Печать стикера для бокса на сервере. ");

  //  PersoClient::ReturnStatus ret =
  //  Client->requestBoxStickerPrint(*param.get()); if (ret !=
  //  PersoClient::NoError) {
  //    emit executionFinished("printBoxStickerOnServer", ret);
  //    return;
  //  }

  //  emit executionFinished("printBoxStickerOnServer", ReturnStatus::NoError);
}

void AdminManager::printLastBoxStickerOnServer() {
  //  emit executionStarted("printLastBoxStickerOnServer");
  //  sendLog("Повторная печать последнего стикера для бокса на сервере. ");

  //  PersoClient::ReturnStatus ret = Client->requestBoxStickerReprint();
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "printLastBoxStickerOnServer");
  //    return;
  //  }

  //  emit executionFinished("printLastBoxStickerOnServer",
  //  ReturnStatus::NoError);
}

void AdminManager::printPalletStickerOnServer(
    std::shared_ptr<StringDictionary> param) {
  //  emit executionStarted("printPalletStickerOnServer");
  //  sendLog("Печать стикера для паллеты на сервере. ");

  //  PersoClient::ReturnStatus ret =
  //      Client->requestPalletStickerPrint(*param.get());
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "printPalletStickerOnServer");
  //    return;
  //  }

  //  emit executionFinished("printPalletStickerOnServer",
  //  ReturnStatus::NoError);
}

void AdminManager::printLastPalletStickerOnServer() {
  //  emit executionStarted("printLastPalletStickerOnServer");
  //  sendLog("Повторная печать последнего стикера для паллеты на сервере. ");

  //  PersoClient::ReturnStatus ret = Client->requestPalletStickerReprint();
  //  if (ret != PersoClient::NoError) {
  //    processClientError(ret, "printLastPalletStickerOnServer");
  //    return;
  //  }

  //  emit executionFinished("printLastPalletStickerOnServer",
  //                         ReturnStatus::NoError);
}

void AdminManager::printTransponderSticker(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  //  emit executionStarted("printTransponderSticker");

  //  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  //  ReturnStatus administratorStatus;

  //  StringDictionary transponderData;
  //  sendLog("Запрос данных транспондера. ");
  //  administratorStatus =
  //      Administrator->getTransponderData(param->value("id"),
  //      &transponderData);
  //  if (administratorStatus != ReturnStatus::NoError) {
  //    processAdministratorError(administratorStatus,
  //    "printTransponderSticker"); return;
  //  }

  //  sendLog("Печать стикера транспондера. ");
  //  stickerPrinterStatus =
  //      StickerPrinter->printTransponderSticker(&transponderData);
  //  if (stickerPrinterStatus != IStickerPrinter::NoError) {
  //    processStickerPrinterError(stickerPrinterStatus,
  //    "printTransponderSticker"); return;
  //  }

  //  emit executionFinished("printTransponderSticker", ReturnStatus::NoError);
}

void AdminManager::printBoxSticker(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("printBoxSticker");

  ReturnStatus ret;

  StringDictionary boxData;
  sendLog("Запрос данных бокса. ");
  ret = Administrator->getBoxData(param->value("id"), boxData);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("printBoxSticker", ret);
    return;
  }

  sendLog("Печать стикера бокса. ");
  ret = StickerPrinter->printBoxSticker(boxData);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("printBoxSticker", ret);
    return;
  }

  emit executionFinished("printBoxSticker", ReturnStatus::NoError);
}

void AdminManager::printPalletSticker(
    const std::shared_ptr<StringDictionary> param,
    SqlQueryValues* model) {
  emit executionStarted("printPalletSticker");

  ReturnStatus ret;

  StringDictionary palletData;
  sendLog("Запрос данных паллеты. ");
  ret = Administrator->getPalletData(param->value("id"), palletData);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("printPalletSticker", ret);
    return;
  }

  sendLog("Печать стикера паллеты. ");
  ret = StickerPrinter->printPalletSticker(palletData);
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("printPalletSticker", ret);
    return;
  }

  emit executionFinished("printPalletSticker", ReturnStatus::NoError);
}

void AdminManager::execPrinterStickerCommandScript(
    const std::shared_ptr<QStringList> commandScript) {
  emit executionStarted("execPrinterStickerCommandScript");

  ReturnStatus ret;

  sendLog("Выполнение командного скрипта принтера стикеров. ");
  ret = StickerPrinter->exec(*commandScript.get());
  if (ret != ReturnStatus::NoError) {
    emit executionFinished("execPrinterStickerCommandScript",
                           ReturnStatus::NoError);
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
}

void AdminManager::createClient() {}

void AdminManager::createStickerPrinter() {
  StickerPrinter = std::unique_ptr<AbstractStickerPrinter>(
      new TE310Printer("RandomPrinter"));
}


//==================================================================================
