#include "admin_manager.h"

/* Диспетчер DSRC  */
//==================================================================================
AdminManager::AdminManager(QObject* parent) : QObject(parent) {
  setObjectName("AdminManager");
  loadSettings();
}

AdminManager::~AdminManager() {
}

void AdminManager::on_InsctanceThreadStarted_slot() {
  // Создаем администратора
  createAdministrator();

  // Создаем клиента
  createClient();

  // Создаем стикер принтер
  createStickerPrinter();
}

void AdminManager::connectDatabase() {
  // Начинаем выполнение операции
  startOperationPerforming("connectDatabase");

  AdministrationSystem::ReturnStatus status;

  sendLog("Подключение к базе данных. ");
  status = Administrator->connectDatabase();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "connectDatabase");
    return;
  }

  finishOperationPerforming("connectDatabase");
}

void AdminManager::disconnectDatabase() {
  // Начинаем выполнение операции
  startOperationPerforming("disconnectDatabase");

  AdministrationSystem::ReturnStatus status;

  sendLog("Отключение от базы данных. ");
  status = Administrator->connectDatabase();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "disconnectDatabase");
    return;
  }

  finishOperationPerforming("disconnectDatabase");
}

void AdminManager::showDatabaseTable(const QString& name,
                                     DatabaseTableModel* model) {
  // Начинаем выполнение операции
  startOperationPerforming("showDatabaseTable");

  AdministrationSystem::ReturnStatus status;

  model->clear();
  sendLog(QString("Отображение таблицы %1. ").arg(name));
  status = Administrator->getDatabaseTable(name, model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "showDatabaseTable");
    return;
  }

  finishOperationPerforming("showDatabaseTable");
}

void AdminManager::clearDatabaseTable(const QString& name,
                                      DatabaseTableModel* model) {
  // Начинаем выполнение операции
  startOperationPerforming("clearDatabaseTable");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Очистка таблицы %1. ").arg(name));
  status = Administrator->clearDatabaseTable(name);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "clearDatabaseTable");
    return;
  }

  model->clear();
  sendLog("Отображение таблицы базы данных. ");
  status = Administrator->getDatabaseTable(name, model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "clearDatabaseTable");
    return;
  }

  finishOperationPerforming("clearDatabaseTable");
}

void AdminManager::performCustomRequest(const QString& req,
                                        DatabaseTableModel* model) {
  // Начинаем выполнение операции
  startOperationPerforming("performCustomRequest");

  AdministrationSystem::ReturnStatus status;

  model->clear();
  sendLog("Представление ответа на кастомный запрос. ");
  status = Administrator->getCustomResponse(req, model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "performCustomRequest");
    return;
  }

  finishOperationPerforming("performCustomRequest");
}

void AdminManager::createNewOrder(
    const QSharedPointer<QHash<QString, QString>> orderParameters,
    DatabaseTableModel* model) {
  startOperationPerforming("createNewOrder");

  AdministrationSystem::ReturnStatus status;

  sendLog("Создание нового заказа. ");
  status = Administrator->createNewOrder(orderParameters);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "createNewOrder");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getDatabaseTable("orders", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "createNewOrder");
    return;
  }

  finishOperationPerforming("createNewOrder");
}

void AdminManager::deleteLastOrder(DatabaseTableModel* model) {
  startOperationPerforming("deleteLastOrder");

  AdministrationSystem::ReturnStatus status;

  sendLog("Удаление последнего заказа. ");
  status = Administrator->deleteLastOrder();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "deleteLastOrder");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getDatabaseTable("orders", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "deleteLastOrder");
    return;
  }

  finishOperationPerforming("deleteLastOrder");
}

void AdminManager::startOrderAssembling(const QString& orderId,
                                        DatabaseTableModel* model) {
  startOperationPerforming("startOrderAssembling");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Запуск сборки заказа %1. ").arg(orderId));
  status = Administrator->startOrderAssembling(orderId);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "startOrderAssembling");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getDatabaseTable("orders", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "startOrderAssembling");
    return;
  }

  finishOperationPerforming("startOrderAssembling");
}

void AdminManager::stopOrderAssembling(const QString& orderId,
                                       DatabaseTableModel* model) {
  startOperationPerforming("stopOrderAssembling");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Остановка сборки заказа %1. ").arg(orderId));
  status = Administrator->stopOrderAssembling(orderId);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "stopOrderAssembling");
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  status = Administrator->getDatabaseTable("orders", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "stopOrderAssembling");
    return;
  }

  finishOperationPerforming("stopOrderAssembling");
}

void AdminManager::showOrderTable(DatabaseTableModel* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::createNewProductionLine(
    const QHash<QString, QString>* productionLineParameters,
    DatabaseTableModel* model) {
  startOperationPerforming("createNewProductionLine");

  AdministrationSystem::ReturnStatus status;

  sendLog("Создание новой линии производства. ");
  status = Administrator->createNewProductionLine(productionLineParameters);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "createNewProductionLine");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getDatabaseTable("production_lines", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "createNewProductionLine");
    return;
  }

  finishOperationPerforming("createNewProductionLine");
}

void AdminManager::allocateInactiveProductionLines(const QString& orderId,
                                                   DatabaseTableModel* model) {
  startOperationPerforming("allocateInactiveProductionLines");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Распределение неактивных линий производства в заказе %1. ")
              .arg(orderId));
  status = Administrator->allocateInactiveProductionLines(orderId);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "allocateInactiveProductionLines");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getDatabaseTable("production_lines", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "allocateInactiveProductionLines");
    return;
  }

  finishOperationPerforming("allocateInactiveProductionLines");
}

void AdminManager::shutdownAllProductionLines(DatabaseTableModel* model) {
  startOperationPerforming("shutdownAllProductionLines");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Остановка всех производственных линий. "));
  status = Administrator->shutdownAllProductionLines();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "shutdownAllProductionLines");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getDatabaseTable("production_lines", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "shutdownAllProductionLines");
    return;
  }

  finishOperationPerforming("shutdownAllProductionLines");
}

void AdminManager::deleteLastProductionLine(DatabaseTableModel* model) {
  startOperationPerforming("deleteLastProductionLine");

  AdministrationSystem::ReturnStatus status;

  sendLog("Удаление последней линии производства. ");
  status = Administrator->deleteLastProductionLine();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "deleteLastProductionLine");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getDatabaseTable("production_lines", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "deleteLastProductionLine");
    return;
  }

  finishOperationPerforming("deleteLastProductionLine");
}

void AdminManager::showProductionLineTable(DatabaseTableModel* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::linkProductionLineWithBox(
    const QHash<QString, QString>* parameters,
    DatabaseTableModel* model) {
  startOperationPerforming("linkProductionLineWithBoxManually");

  AdministrationSystem::ReturnStatus status;

  sendLog("Связывание линии производства с определенным боксом. ");
  status = Administrator->linkProductionLineWithBox(parameters);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "linkProductionLineWithBox");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  status = Administrator->getDatabaseTable("production_lines", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "linkProductionLineWithBox");
    return;
  }

  finishOperationPerforming("linkProductionLineWithBox");
}

void AdminManager::initIssuers(DatabaseTableModel* model) {
  startOperationPerforming("initIssuers");

  AdministrationSystem::ReturnStatus status;

  sendLog("Инициализация данных об эмитентах. ");
  status = Administrator->initIssuerTable();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "initIssuers");
    return;
  }

  model->clear();
  sendLog("Отображение эмитентов. ");
  status = Administrator->getDatabaseTable("issuers", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "initIssuers");
    return;
  }

  finishOperationPerforming("initIssuers");
}

void AdminManager::initTransportMasterKeys(DatabaseTableModel* model) {
  startOperationPerforming("initTransportMasterKeys");

  AdministrationSystem::ReturnStatus status;

  sendLog("Инициализация транспортных мастер ключей. ");
  status = Administrator->initTransportMasterKeysTable();
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "initTransportMasterKeys");
    return;
  }

  model->clear();
  sendLog("Отображение транспортных мастер ключей. ");
  status = Administrator->getDatabaseTable("transport_master_keys", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "initTransportMasterKeys");
    return;
  }

  finishOperationPerforming("initTransportMasterKeys");
}

void AdminManager::linkIssuerWithMasterKeys(
    DatabaseTableModel* model,
    const QHash<QString, QString>* parameters) {
  startOperationPerforming("linkIssuerWithMasterKeys");

  AdministrationSystem::ReturnStatus status;

  sendLog(QString("Связывание эмитента %1 с мастер ключами %2. ")
              .arg(parameters->value("issuer_id"),
                   parameters->value("master_keys_id")));
  status = Administrator->linkIssuerWithMasterKeys(parameters);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "linkIssuerWithMasterKeys");
    return;
  }

  model->clear();
  sendLog("Отображение таблицы эмитентов. ");
  status = Administrator->getDatabaseTable("issuers", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "linkIssuerWithMasterKeys");
    return;
  }

  finishOperationPerforming("linkIssuerWithMasterKeys");
}

void AdminManager::releaseTranspondersManually(
    const QSharedPointer<QHash<QString, QString>> param,
    DatabaseTableModel* model) {
  startOperationPerforming("releaseTranspondersManually");
  sendLog("Принудительный выпуск транспондеров. ");

  AdministrationSystem::ReturnStatus status;

  status = Administrator->releaseTranspondersManually(param->value("table"),
                                                      param->value("id"));
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "releaseTranspondersManually");
    return;
  }

  status = Administrator->getDatabaseTable(param->value("table"), model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "releaseTranspondersManually");
    return;
  }

  finishOperationPerforming("releaseTranspondersManually");
}

void AdminManager::refundTranspondersManually(
    const QSharedPointer<QHash<QString, QString>> param,
    DatabaseTableModel* model) {
  startOperationPerforming("refundTranspondersManually");
  sendLog("Возврат транспондеров. ");

  AdministrationSystem::ReturnStatus status;

  status = Administrator->refundTranspondersManually(param->value("table"),
                                                     param->value("id"));
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "refundTranspondersManually");
    return;
  }

  status = Administrator->getDatabaseTable(param->value("table"), model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "refundTranspondersManually");
    return;
  }

  finishOperationPerforming("refundTranspondersManually");
}

void AdminManager::shipPallets(
    const QSharedPointer<QHash<QString, QString>> param,
    DatabaseTableModel* model) {
  startOperationPerforming("shipPallets");
  sendLog("Отгрузка паллет. ");

  AdministrationSystem::ReturnStatus status;
  status = Administrator->shipPallets(param.get());
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "shipPallets");
    return;
  }

  status = Administrator->getDatabaseTable("pallets", model);
  if (status != AdministrationSystem::Completed) {
    processAdministratorError(status, "shipPallets");
    return;
  }

  finishOperationPerforming("shipPallets");
}

void AdminManager::releaseTransponder(
    const QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("releaseTransponder");
  sendLog("Выпуск транспондера. ");

  QSharedPointer<QFile> firmware(new QFile("temp.bin"));
  QSharedPointer<QHash<QString, QString>> transponderData(
      new QHash<QString, QString>());
  PersoClient::ReturnStatus status = Client->requestTransponderRelease(
      param.get(), firmware.get(), transponderData.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "releaseTransponder");
    return;
  }

  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  emit displayFirmware_signal(firmware);

  sendLog("Запрос на отображение данных транспондера. ");
  emit displayTransponderData_signal(transponderData);

  finishOperationPerforming("releaseTransponder");
}

void AdminManager::confirmTransponderRelease(
    const QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("confirmTransponderRelease");
  sendLog("Подтверждение выпуска транспондера. ");

  PersoClient::ReturnStatus status =
      Client->requestTransponderReleaseConfirm(param.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "confirmTransponderRelease");
    return;
  }

  finishOperationPerforming("confirmTransponderRelease");
}

void AdminManager::rereleaseTransponder(
    const QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("rereleaseTransponder");
  sendLog("Перевыпуск транспондера. ");

  QSharedPointer<QFile> firmware(new QFile("temp.bin"));
  QSharedPointer<QHash<QString, QString>> transponderData(
      new QHash<QString, QString>());
  PersoClient::ReturnStatus status = Client->requestTransponderRerelease(
      param.get(), firmware.get(), transponderData.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "releaseTransponder");
    return;
  }

  sendLog("Запрос на отображение полученой прошивки транспондера. ");
  emit displayFirmware_signal(firmware);
  sendLog("Запрос на отображение данных транспондера. ");
  emit displayTransponderData_signal(transponderData);

  finishOperationPerforming("rereleaseTransponder");
}

void AdminManager::confirmTransponderRerelease(
    const QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("confirmTransponderRerelease");
  sendLog("Подтверждение перевыпуска транспондера. ");

  PersoClient::ReturnStatus status =
      Client->requestTransponderRereleaseConfirm(param.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "confirmTransponderRelease");
    return;
  }

  finishOperationPerforming("confirmTransponderRerelease");
}

void AdminManager::rollbackProductionLine(
    const QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("rollbackProductionLine");
  sendLog("Откат производственной линии. ");

  PersoClient::ReturnStatus status =
      Client->requestProductionLineRollback(param.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "rollbackProductionLine");
    return;
  }

  finishOperationPerforming("rollbackProductionLine");
}

void AdminManager::printBoxStickerOnServer(
    QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("printBoxStickerOnServer");
  sendLog("Печать стикера для бокса на сервере. ");

  PersoClient::ReturnStatus status =
      Client->requestBoxStickerPrint(param.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "printBoxStickerOnServer");
    return;
  }

  finishOperationPerforming("printBoxStickerOnServer");
}

void AdminManager::printLastBoxStickerOnServer() {
  startOperationPerforming("printLastBoxStickerOnServer");
  sendLog("Повторная печать последнего стикера для бокса на сервере. ");

  PersoClient::ReturnStatus status = Client->requestBoxStickerReprint();
  if (status != PersoClient::Completed) {
    processClientError(status, "printLastBoxStickerOnServer");
    return;
  }

  finishOperationPerforming("printLastBoxStickerOnServer");
}

void AdminManager::printPalletStickerOnServer(
    QSharedPointer<QHash<QString, QString>> param) {
  startOperationPerforming("printPalletStickerOnServer");
  sendLog("Печать стикера для паллеты на сервере. ");

  PersoClient::ReturnStatus status =
      Client->requestPalletStickerPrint(param.get());
  if (status != PersoClient::Completed) {
    processClientError(status, "printPalletStickerOnServer");
    return;
  }

  finishOperationPerforming("printPalletStickerOnServer");
}

void AdminManager::printLastPalletStickerOnServer() {
  startOperationPerforming("printLastPalletStickerOnServer");
  sendLog("Повторная печать последнего стикера для паллеты на сервере. ");

  PersoClient::ReturnStatus status = Client->requestPalletStickerReprint();
  if (status != PersoClient::Completed) {
    processClientError(status, "printLastPalletStickerOnServer");
    return;
  }

  finishOperationPerforming("printLastPalletStickerOnServer");
}

void AdminManager::printTransponderSticker(const QString& id,
                                           DatabaseTableModel* model) {
  startOperationPerforming("printTransponderSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  AdministrationSystem::ReturnStatus administratorStatus;

  QHash<QString, QString> transponderData;
  sendLog("Запрос данных транспондера. ");
  administratorStatus = Administrator->getTransponderData(id, &transponderData);
  if (administratorStatus != AdministrationSystem::Completed) {
    processAdministratorError(administratorStatus, "printTransponderSticker");
    return;
  }

  sendLog("Печать стикера транспондера. ");
  stickerPrinterStatus =
      StickerPrinter->printTransponderSticker(&transponderData);
  if (stickerPrinterStatus != IStickerPrinter::Completed) {
    processStickerPrinterError(stickerPrinterStatus, "printTransponderSticker");
    return;
  }

  finishOperationPerforming("printTransponderSticker");
}

void AdminManager::printBoxSticker(const QString& id,
                                   DatabaseTableModel* model) {
  startOperationPerforming("printBoxSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  AdministrationSystem::ReturnStatus administratorStatus;

  QHash<QString, QString> boxData;
  sendLog("Запрос данных бокса. ");
  administratorStatus = Administrator->getBoxData(id, &boxData);
  if (administratorStatus != AdministrationSystem::Completed) {
    processAdministratorError(administratorStatus, "printBoxSticker");
    return;
  }

  sendLog("Печать стикера бокса. ");
  stickerPrinterStatus = StickerPrinter->printBoxSticker(&boxData);
  if (stickerPrinterStatus != IStickerPrinter::Completed) {
    processStickerPrinterError(stickerPrinterStatus, "printBoxSticker");
    return;
  }

  finishOperationPerforming("printBoxSticker");
}

void AdminManager::printPalletSticker(const QString& id,
                                      DatabaseTableModel* model) {
  startOperationPerforming("printPalletSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  AdministrationSystem::ReturnStatus administratorStatus;

  QHash<QString, QString> palletData;
  sendLog("Запрос данных паллеты. ");
  administratorStatus = Administrator->getPalletData(id, &palletData);
  if (administratorStatus != AdministrationSystem::Completed) {
    processAdministratorError(administratorStatus, "printTransponderSticker");
    return;
  }

  sendLog("Печать стикера паллеты. ");
  stickerPrinterStatus = StickerPrinter->printPalletSticker(&palletData);
  if (stickerPrinterStatus != IStickerPrinter::Completed) {
    processStickerPrinterError(stickerPrinterStatus, "printPalletSticker");
    return;
  }

  finishOperationPerforming("printPalletSticker");
}

void AdminManager::execPrinterStickerCommandScript(
    const QSharedPointer<QStringList> commandScript) {
  startOperationPerforming("execPrinterStickerCommandScript");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;

  sendLog("Выполнение командного скрипта принтера стикеров. ");
  stickerPrinterStatus = StickerPrinter->exec(commandScript.get());
  if (stickerPrinterStatus != IStickerPrinter::Completed) {
    processStickerPrinterError(stickerPrinterStatus,
                               "execPrinterStickerCommandScript");
    return;
  }

  finishOperationPerforming("execPrinterStickerCommandScript");
}

void AdminManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Administrator->applySettings();
  Client->applySettings();
  StickerPrinter->applySetting();
}

void AdminManager::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("log_system/global_enable").toBool();
}

void AdminManager::sendLog(const QString& log) const {
  if (LogEnable) {
    emit logging(QString("%1 - %2").arg(objectName(), log));
  }
}

void AdminManager::createAdministrator() {
  Administrator = new AdministrationSystem(this);
  connect(Administrator, &AdministrationSystem::logging, LogSystem::instance(),
          &LogSystem::generate);

  // Заполняем таблицу соответствий статусов возврата
  AdministratorReturnStatusMatch.insert(AdministrationSystem::NotExecuted,
                                        "Выполнение операции не началось.");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::ParameterError,
                                        "Получены некорректные параметры.");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseConnectionError,
      "Не удалось подключиться к базе данных. ");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseTransactionError, "Ошибка транзакции. ");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseQueryError,
      "Получена ошибка при выполнении запроса к базе данных.");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::ShipmentRegisterError,
      "Не удалось открыть файл-реестр для отгрузки.");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::LogicError,
                                        "Логическая ошибка.");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::UnknownError,
                                        "Неизвествная ошибка. ");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::Completed,
                                        "Выполнено. ");
}

void AdminManager::createClient() {
  Client = new PersoClient(this);
  connect(Client, &PersoClient::logging, LogSystem::instance(),
          &LogSystem::generate);

  // Заполняем таблицу соответствий статусов возврата
  ClientReturnStatusMatch.insert(PersoClient::Undefined,
                                 "Выполнение операции не началось.");
  ClientReturnStatusMatch.insert(PersoClient::RequestParameterError,
                                 "Не удалось обработать параметры запроса. ");
  ClientReturnStatusMatch.insert(PersoClient::ServerConnectionError,
                                 "Не удалось подключиться к серверу. ");
  ClientReturnStatusMatch.insert(PersoClient::ServerNotResponding,
                                 "Сервер не отвечает.");
  ClientReturnStatusMatch.insert(PersoClient::ServerConnectionTerminated,
                                 "Оборвалось соединение с сервером.");
  ClientReturnStatusMatch.insert(PersoClient::AuthorizationNotExist,
                                 "Производственная линия не существует.");
  ClientReturnStatusMatch.insert(PersoClient::AuthorizationAccessDenied,
                                 "Ошибка доступа к производственной линии.");
  ClientReturnStatusMatch.insert(PersoClient::AuthorizationNotActive,
                                 "Производственная линия не активна. ");
  ClientReturnStatusMatch.insert(PersoClient::ResponseSyntaxError,
                                 "Синтаксическая ошибка в ответе на запрос. ");
  ClientReturnStatusMatch.insert(PersoClient::ServerError,
                                 "Серверная ошибка. ");
  ClientReturnStatusMatch.insert(PersoClient::UnknownError,
                                 "Неизвествная ошибка. ");
  ClientReturnStatusMatch.insert(PersoClient::Completed, "Выполнено. ");
}

void AdminManager::createStickerPrinter() {
  StickerPrinter = new TE310Printer(this, "RandomPrinter");
  connect(StickerPrinter, &IStickerPrinter::logging, LogSystem::instance(),
          &LogSystem::generate);

  // Заполняем таблицу соответствий статусов возврата
  StickerPrinterReturnStatusMatch.insert(
      IStickerPrinter::ParameterError,
      "Получены некорректные параметры для стикера.");
  StickerPrinterReturnStatusMatch.insert(IStickerPrinter::Failed,
                                         "Не удалось распечать стикер.");
  StickerPrinterReturnStatusMatch.insert(
      IStickerPrinter::LibraryMissed,
      "Отсутствует библиотека для работы с принтером стикеров.");
  StickerPrinterReturnStatusMatch.insert(IStickerPrinter::ConnectionError,
                                         "Не удалось подключиться к принтеру.");
  StickerPrinterReturnStatusMatch.insert(IStickerPrinter::Completed,
                                         "Выполнено.");
}

void AdminManager::startOperationPerforming(const QString& operationName) {
  Mutex.lock();

  emit operationPerfomingStarted(operationName);
}

void AdminManager::finishOperationPerforming(const QString& operationName) {
  // Сигнал о завершении текущей операции
  emit operationPerformingFinished(operationName);

  // Опопвещаем пользователя
  emit notifyUser("Выполнено.");

  // Разблокируем мьютекс
  Mutex.unlock();
}

void AdminManager::processAdministratorError(
    AdministrationSystem::ReturnStatus status,
    const QString& operationName) {
  sendLog(AdministratorReturnStatusMatch.value(status));
  emit operationPerformingFinished(operationName);
  emit notifyUserAboutError(AdministratorReturnStatusMatch.value(status));
  Mutex.unlock();
}

void AdminManager::processClientError(PersoClient::ReturnStatus status,
                                      const QString& operationName) {
  sendLog(ClientReturnStatusMatch.value(status));
  emit operationPerformingFinished(operationName);
  emit notifyUserAboutError(ClientReturnStatusMatch.value(status));
  Mutex.unlock();
}

void AdminManager::processStickerPrinterError(
    IStickerPrinter::ReturnStatus status,
    const QString& operationName) {
  sendLog(StickerPrinterReturnStatusMatch.value(status));
  emit operationPerformingFinished(operationName);
  emit notifyUserAboutError(StickerPrinterReturnStatusMatch.value(status));
  Mutex.unlock();
}

//==================================================================================
