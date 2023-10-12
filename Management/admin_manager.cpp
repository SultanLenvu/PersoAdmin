#include "admin_manager.h"

/* Диспетчер DSRC  */
//==================================================================================
AdminManager::AdminManager(QObject* parent) : QObject(parent) {
  setObjectName("AdminManager");
  loadSettings();

  // Готовы к выполнению операций
  CurrentState = Ready;
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
  if (!startOperationExecution("connectDatabase")) {
    return;
  }

  sendLog("Подключение к базе данных. ");
  if (Administrator->connectDatabase() != AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("connectDatabase",
                             "Получена ошибка при подключении к базе данных. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("connectDatabase", "Выполнено. ");
}

void AdminManager::disconnectDatabase() {
  // Начинаем выполнение операции
  if (!startOperationExecution("disconnectDatabase")) {
    return;
  }

  sendLog("Отключение от базы данных. ");
  if (Administrator->connectDatabase() != AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("disconnectDatabase",
                             "Получена ошибка при отключении от базы данных. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("disconnectDatabase", "Выполнено. ");
}

void AdminManager::showDatabaseTable(const QString& name,
                                     DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("showDatabaseTable")) {
    return;
  }

  model->clear();
  sendLog(QString("Отображение таблицы %1. ").arg(name));
  if (Administrator->getDatabaseTable(name, model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "showDatabaseTable",
        QString("Получена ошибка при загрузке данных из таблицы '%1'. ")
            .arg(name));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("showDatabaseTable", "Выполнено. ");
}

void AdminManager::clearDatabaseTable(const QString& name,
                                      DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("clearDatabaseTable")) {
    return;
  }

  sendLog(QString("Очистка таблицы %1. ").arg(name));
  if (Administrator->clearDatabaseTable(name) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "clearDatabaseTable",
        QString("Получена ошибка при очистке таблицы %1. ").arg(name));
    return;
  }

  model->clear();
  sendLog("Отображение таблицы базы данных. ");
  if (Administrator->getDatabaseTable(name, model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "clearDatabaseTable",
        QString("Получена ошибка при загрузке данных из таблицы '%1'. ")
            .arg(name));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("clearDatabaseTable", "Выполнено. ");
}

void AdminManager::performCustomRequest(const QString& req,
                                        DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("performCustomRequest")) {
    return;
  }

  model->clear();
  sendLog("Представление ответа на кастомный запрос. ");
  if (Administrator->getCustomResponse(req, model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "performCustomRequest",
        QString("Получена ошибка при выполнении произвольного запроса. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("performCustomRequest", "Выполнено. ");
}

void AdminManager::createNewOrder(
    const QSharedPointer<QMap<QString, QString> > orderParameters,
    DatabaseTableModel* model) {
  if (!startOperationExecution("createNewOrder")) {
    return;
  }

  sendLog("Создание нового заказа. ");
  if (Administrator->createNewOrder(orderParameters) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "createNewOrder",
        QString("Получена ошибка при добавлении нового заказа. "));
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  if (Administrator->getDatabaseTable("orders", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "createNewOrder",
        QString("Получена ошибка при загрузке данных из таблицы заказов. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("createNewOrder", "Выполнено. ");
}

void AdminManager::deleteLastOrder(DatabaseTableModel* model) {
  if (!startOperationExecution("deleteLastOrder")) {
    return;
  }

  sendLog("Удаление последнего заказа. ");
  if (Administrator->deleteLastOrder() != AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "deleteLastOrder",
        QString("Получена ошибка при удалении последнего заказа. "));
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  if (Administrator->getDatabaseTable("orders", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "deleteLastOrder",
        QString("Получена ошибка при загрузке данных из таблицы заказов. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("deleteLastOrder", "Выполнено. ");
}

void AdminManager::startOrderAssembling(const QString& orderId,
                                        DatabaseTableModel* model) {
  if (!startOperationExecution("startOrderAssembling")) {
    return;
  }

  sendLog(QString("Запуск сборки заказа %1. ").arg(orderId));
  if (Administrator->startOrderAssembling(orderId) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "startOrderAssembling",
        QString("Получена ошибка при запуске сборки заказа %1. ").arg(orderId));
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  if (Administrator->getDatabaseTable("orders", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "startOrderAssembling",
        QString("Получена ошибка при загрузке данных из таблицы заказов. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("startOrderAssembling", "Выполнено. ");
}

void AdminManager::stopOrderAssembling(const QString& orderId,
                                       DatabaseTableModel* model) {
  if (!startOperationExecution("stopOrderAssembling")) {
    return;
  }

  sendLog(QString("Остановка сборки заказа %1. ").arg(orderId));
  if (Administrator->stopOrderAssembling(orderId) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "stopOrderAssembling",
        QString("Получена ошибка при остановке сборки заказа %1. ")
            .arg(orderId));
    return;
  }

  model->clear();
  sendLog("Отображение заказов. ");
  if (Administrator->getDatabaseTable("orders", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "stopOrderAssembling",
        QString("Получена ошибка при загрузке данных из таблицы заказов. ")
            .arg(orderId));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("stopOrderAssembling", "Выполнено. ");
}

void AdminManager::showOrderTable(DatabaseTableModel* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::createNewProductionLine(
    const QMap<QString, QString>* productionLineParameters,
    DatabaseTableModel* model) {
  if (!startOperationExecution("createNewProductionLine")) {
    return;
  }

  sendLog("Создание новой линии производства. ");
  if (Administrator->createNewProductionLine(productionLineParameters) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "createNewProductionLine",
        QString("Получена ошибка при создании новой линии производства. "));
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  if (Administrator->getDatabaseTable("production_lines", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("createNewProductionLine",
                             QString("Получена ошибка при загрузке данных из "
                                     "таблицы линий производства. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("createNewProductionLine", "Выполнено. ");
}

void AdminManager::allocateInactiveProductionLines(const QString& orderId,
                                                   DatabaseTableModel* model) {
  if (!startOperationExecution("allocateInactiveProductionLines")) {
    return;
  }

  sendLog(QString("Распределение неактивных линий производства в заказе %1. ")
              .arg(orderId));
  if (Administrator->allocateInactiveProductionLines(orderId) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "allocateInactiveProductionLines",
        QString("Получена ошибка при распределении неактивных линий "
                "производства в заказе %1. ")
            .arg(orderId));
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  if (Administrator->getDatabaseTable("production_lines", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("allocateInactiveProductionLines",
                             QString("Получена ошибка при загрузке данных из "
                                     "таблицы производственных линий. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("allocateInactiveProductionLines", "Выполнено. ");
}

void AdminManager::shutdownAllProductionLines(DatabaseTableModel* model) {
  if (!startOperationExecution("shutdownAllProductionLines")) {
    return;
  }

  sendLog(QString("Остановка всех производственных линий. "));
  if (Administrator->shutdownAllProductionLines() !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "shutdownAllProductionLines",
        QString("Получена ошибка при остановке всех производственных линий. "));
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  if (Administrator->getDatabaseTable("production_lines", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("shutdownAllProductionLines",
                             QString("Получена ошибка при загрузке данных из "
                                     "таблицы производственных линий. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("shutdownAllProductionLines", "Выполнено. ");
}

void AdminManager::deleteLastProductionLine(DatabaseTableModel* model) {
  if (!startOperationExecution("deleteLastProductionLine")) {
    return;
  }

  sendLog("Удаление последней линии производства. ");
  if (Administrator->deleteLastProductionLine() !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("deleteLastProductionLine",
                             QString("Получена ошибка при удалении последней "
                                     "созданной производственной линии. "));
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  if (Administrator->getDatabaseTable("production_lines", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("deleteLastProductionLine",
                             QString("Получена ошибка при загрузке данных из "
                                     "таблицы производственных линий. "));
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("deleteLastProductionLine", "Выполнено. ");
}

void AdminManager::showProductionLineTable(DatabaseTableModel* model) {
  showDatabaseTable("orders", model);
}

void AdminManager::linkProductionLineWithBox(
    const QMap<QString, QString>* parameters,
    DatabaseTableModel* model) {
  if (!startOperationExecution("linkProductionLineWithBoxManually")) {
    return;
  }

  sendLog("Связывание линии производства с определенным боксом. ");
  if (Administrator->linkProductionLineWithBox(parameters)) {
    CurrentState = Failed;
    finishOperationExecution(
        "linkProductionLineWithBox",
        "Получена ошибка при связывании линии производства с "
        "определенным боксом. ");
    return;
  }

  model->clear();
  sendLog("Отображение производственных линий. ");
  if (Administrator->getDatabaseTable("production_lines", model)) {
    CurrentState = Failed;
    finishOperationExecution("linkProductionLineWithBox",
                             "Получена ошибка при загрузке данных из таблицы "
                             "производственных линий. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("linkProductionLineWithBox", "Выполнено. ");
}

void AdminManager::initIssuers(DatabaseTableModel* model) {
  if (!startOperationExecution("initIssuers")) {
    return;
  }

  sendLog("Инициализация данных об эмитентах. ");
  if (Administrator->initIssuerTable()) {
    CurrentState = Failed;
    finishOperationExecution(
        "initIssuers", "Получена ошибка при инициализации таблицы эмитентов. ");
    return;
  }

  model->clear();
  sendLog("Отображение эмитентов. ");
  if (Administrator->getDatabaseTable("issuers", model)) {
    CurrentState = Failed;
    finishOperationExecution(
        "initIssuers",
        "Получена ошибка при загрузке данных из таблицы эмитентов. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("initIssuers", "Выполнено. ");
}

void AdminManager::initTransportMasterKeys(DatabaseTableModel* model) {
  if (!startOperationExecution("initTransportMasterKeys")) {
    return;
  }

  sendLog("Инициализация транспортных мастер ключей. ");
  if (Administrator->initTransportMasterKeysTable()) {
    CurrentState = Failed;
    finishOperationExecution(
        "initTransportMasterKeys",
        "Получена ошибка при инициализации транспортных мастер ключей. ");
    return;
  }

  model->clear();
  sendLog("Отображение транспортных мастер ключей. ");
  if (Administrator->getDatabaseTable("transport_master_keys", model)) {
    CurrentState = Failed;
    finishOperationExecution(
        "initTransportMasterKeys",
        "Получена ошибка при загрузке данных из таблицы мастер ключей. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("initTransportMasterKeys", "Выполнено. ");
}

void AdminManager::linkIssuerWithMasterKeys(
    DatabaseTableModel* model,
    const QMap<QString, QString>* parameters) {
  if (!startOperationExecution("linkIssuerWithMasterKeys")) {
    return;
  }

  sendLog(QString("Связывание эмитента %1 с мастер ключами %2. ")
              .arg(parameters->value("issuer_id"),
                   parameters->value("master_keys_id")));
  if (Administrator->linkIssuerWithMasterKeys(parameters) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "linkIssuerWithMasterKeys",
        QString(
            "Получена ошибка при связывании эмитента %1 с мастер ключами %2. ")
            .arg(parameters->value("issuer_id"),
                 parameters->value("master_keys_id")));
    return;
  }

  model->clear();
  sendLog("Отображение таблицы эмитентов. ");
  if (Administrator->getDatabaseTable("issuers", model) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "linkIssuerWithMasterKeys",
        "Получена ошибка при загрузке данных из таблицы эмитентов. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("linkIssuerWithMasterKeys", "Выполнено. ");
}

void AdminManager::printTransponderSticker(const QString& id,
                                           DatabaseTableModel* model) {
  if (!startOperationExecution("printTransponderSticker")) {
    return;
  }

  QMap<QString, QString> transponderData;
  if (Administrator->getTransponderData(id, &transponderData) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при получении данных транспондера. ");
    return;
  }

  if (!StickerPrinter->printTransponderSticker(&transponderData)) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при печати стикера транспондера. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("printTransponderSticker", "Выполнено. ");
}

void AdminManager::printBoxSticker(const QString& id,
                                   DatabaseTableModel* model) {
  if (!startOperationExecution("printBoxSticker")) {
    return;
  }

  QMap<QString, QString> boxData;
  if (Administrator->getBoxData(id, &boxData) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при получении данных транспондера. ");
    return;
  }

  if (!StickerPrinter->printBoxSticker(&boxData)) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при печати стикера транспондера. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("printBoxSticker", "Выполнено. ");
}

void AdminManager::printPalletSticker(const QString& id,
                                      DatabaseTableModel* model) {
  if (!startOperationExecution("printPalletSticker")) {
    return;
  }

  QMap<QString, QString> palletData;
  if (Administrator->getPalletData(id, &palletData) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при получении данных транспондера. ");
    return;
  }

  if (!StickerPrinter->printPalletSticker(&palletData)) {
    CurrentState = Failed;
    finishOperationExecution(
        "printTransponderSticker",
        "Получена ошибка при печати стикера транспондера. ");
    return;
  }

  // Завершаем выполнение операции
  CurrentState = Completed;
  finishOperationExecution("printPalletSticker", "Выполнено. ");
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
}

void AdminManager::createStickerPrinter() {
  StickerPrinter = new TE310Printer(this);
  connect(StickerPrinter, &IStickerPrinter::logging, this,
          &AdminManager::proxyLogging);
}

bool AdminManager::startOperationExecution(const QString& operationName) {
  // Проверяем готовность к выполнению операции
  if (CurrentState != Ready) {
    sendLog("Предыдущая операция не завершена. Выполнение новой невозможно. ");
    return false;
  }

  // Переходим в состояние ожидания конца обработки
  CurrentState = WaitingExecution;

  // Отправляем сигнал о начале выполнения длительной операции
  emit operationPerfomingStarted(operationName);

  return true;
}

void AdminManager::finishOperationExecution(const QString& operationName,
                                            const QString& msg) {
  sendLog(msg);

  // Оповещаем пользователя о результатах
  if (CurrentState == Completed) {
    emit notifyUser(msg);
  } else {
    emit notifyUserAboutError(msg);
  }

  // Готовы к следующей операции
  CurrentState = Ready;

  // Сигнал о завершении текущей операции
  emit operationPerformingFinished(operationName);
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
