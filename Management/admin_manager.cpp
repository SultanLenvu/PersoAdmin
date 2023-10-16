#include "admin_manager.h"

/* Диспетчер DSRC  */
//==================================================================================
AdminManager::AdminManager(QObject* parent) : QObject(parent) {
  setObjectName("AdminManager");
  loadSettings();
}

AdminManager::~AdminManager() {
}

void AdminManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Administrator->applySettings();
}

void AdminManager::on_InsctanceThreadStarted_slot() {
  // Создаем администратора
  createAdministrator();

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
    const QSharedPointer<QMap<QString, QString> > orderParameters,
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
    const QMap<QString, QString>* productionLineParameters,
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
    const QMap<QString, QString>* parameters,
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
    const QMap<QString, QString>* parameters) {
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

void AdminManager::printTransponderSticker(const QString& id,
                                           DatabaseTableModel* model) {
  startOperationPerforming("printTransponderSticker");

  IStickerPrinter::ReturnStatus stickerPrinterStatus;
  AdministrationSystem::ReturnStatus administratorStatus;

  QMap<QString, QString> transponderData;
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

  QMap<QString, QString> boxData;
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

  QMap<QString, QString> palletData;
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

void AdminManager::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("log_system/global_enable").toBool();
}

void AdminManager::sendLog(const QString& log) const {
  if (LogEnable) {
    emit logging(log);
  }
}

void AdminManager::createAdministrator() {
  Administrator = new AdministrationSystem(this);
  connect(Administrator, &AdministrationSystem::logging, this,
          &AdminManager::proxyLogging);

  // Заполняем таблицу соответствий статусов возврата
  AdministratorReturnStatusMatch.insert(AdministrationSystem::NotExecuted,
                                        "Выполнение операции не началось.");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseConnectionError,
      "Не удалось подключиться к базе данных. ");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseTransactionError, "Ошибка транзакции. ");
  AdministratorReturnStatusMatch.insert(
      AdministrationSystem::DatabaseQueryError,
      "Получена ошибка при выполнении запроса к базе данных.");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::LogicError,
                                        "Логическая ошибка.");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::UnknownError,
                                        "Неизвествная ошибка. ");
  AdministratorReturnStatusMatch.insert(AdministrationSystem::Completed,
                                        "Выполнено. ");
}

void AdminManager::createStickerPrinter() {
  StickerPrinter = new TE310Printer(this);
  connect(StickerPrinter, &IStickerPrinter::logging, this,
          &AdminManager::proxyLogging);

  // Заполняем таблицу соответствий статусов возврата
  StickerPrinterReturnStatusMatch.insert(
      IStickerPrinter::ParameterError,
      "Получены некорректные параметры для стикера.");
  StickerPrinterReturnStatusMatch.insert(IStickerPrinter::Failed,
                                         "Не удалось распечать стикер.");
  StickerPrinterReturnStatusMatch.insert(
      IStickerPrinter::LibraryMissing,
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
  emit operationPerformingFinished(operationName);
  emit notifyUserAboutError(AdministratorReturnStatusMatch.value(status));
  Mutex.unlock();
}

void AdminManager::processStickerPrinterError(
    IStickerPrinter::ReturnStatus status,
    const QString& operationName) {
  emit operationPerformingFinished(operationName);
  emit notifyUserAboutError(StickerPrinterReturnStatusMatch.value(status));
  Mutex.unlock();
}

/*
 * Приватные слоты
 */

void AdminManager::proxyLogging(const QString& log) {
  if (sender()->objectName() == QString("AdministrationSystem")) {
    sendLog(QString("Administrator - ") + log);
  } else if (sender()->objectName() == QString("TE310Printer")) {
    sendLog(QString("TE310 - ") + log);
  } else {
    sendLog(QString("Unknown - ") + log);
  }
}

//==================================================================================
