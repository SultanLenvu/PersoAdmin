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
  emit logging("Применение новых настроек. ");
  loadSettings();

  Administrator->applySettings();
}

void AdminManager::on_InsctanceThreadStarted() {
  // Создаем среду выполнения для инициализатора
  createAdministrator();

  // Создаем таймеры
  createTimers();
}

void AdminManager::connectDatabase() {
  if (QApplication::instance()->thread() == thread()) {
    emit logging("Код запускается в главном потоке. ");
  } else {
    emit logging("Код запускается в отдельном потоке. ");
  }

  // Начинаем выполнение операции
  if (!startOperationExecution("connectDatabase")) {
    return;
  }

  emit logging("Подключение к базе данных. ");
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

  emit logging("Отключение от базы данных. ");
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
  emit logging(QString("Отображение таблицы %1. ").arg(name));
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

  emit logging(QString("Очистка таблицы %1. ").arg(name));
  if (Administrator->clearDatabaseTable(name) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "clearDatabaseTable",
        QString("Получена ошибка при очистке таблицы %1. ").arg(name));
    return;
  }

  model->clear();
  emit logging("Отображение таблицы базы данных. ");
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
  emit logging("Представление ответа на кастомный запрос. ");
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

void AdminManager::createNewOrder(const QMap<QString, QString>* orderParameters,
                                  DatabaseTableModel* model) {
  if (!startOperationExecution("createNewOrder")) {
    return;
  }

  emit logging("Создание нового заказа. ");
  if (Administrator->createNewOrder(orderParameters) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "createNewOrder",
        QString("Получена ошибка при добавлении нового заказа. "));
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
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

  emit logging("Удаление последнего заказа. ");
  if (Administrator->deleteLastOrder() != AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "deleteLastOrder",
        QString("Получена ошибка при удалении последнего заказа. "));
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
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

  emit logging(QString("Запуск сборки заказа %1. ").arg(orderId));
  if (Administrator->startOrderAssembling(orderId)) {
    CurrentState = Failed;
    finishOperationExecution(
        "startOrderAssembling",
        QString("Получена ошибка при запуске сборки заказа %1. ").arg(orderId));
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  if (Administrator->getDatabaseTable("orders", model)) {
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

  emit logging(QString("Остановка сборки заказа %1. ").arg(orderId));
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
  emit logging("Отображение заказов. ");
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

  emit logging("Создание новой линии производства. ");
  if (Administrator->createNewProductionLine(productionLineParameters) !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "createNewProductionLine",
        QString("Получена ошибка при создании новой линии производства. "));
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
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

  emit logging(
      QString("Распределение неактивных линий производства в заказе %1. ")
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
  emit logging("Отображение производственных линий. ");
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

  emit logging(QString("Остановка всех производственных линий. "));
  if (Administrator->shutdownAllProductionLines() !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution(
        "shutdownAllProductionLines",
        QString("Получена ошибка при остановке всех производственных линий. "));
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
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

  emit logging("Удаление последней линии производства. ");
  if (Administrator->deleteLastProductionLine() !=
      AdministrationSystem::Completed) {
    CurrentState = Failed;
    finishOperationExecution("deleteLastProductionLine",
                             QString("Получена ошибка при удалении последней "
                                     "созданной производственной линии. "));
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
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

  emit logging("Связывание линии производства с определенным боксом. ");
  if (Administrator->linkProductionLineWithBox(parameters)) {
    CurrentState = Failed;
    finishOperationExecution(
        "linkProductionLineWithBox",
        "Получена ошибка при связывании линии производства с "
        "определенным боксом. ");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
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

  emit logging("Инициализация данных об эмитентах. ");
  if (Administrator->initIssuerTable()) {
    CurrentState = Failed;
    finishOperationExecution(
        "initIssuers", "Получена ошибка при инициализации таблицы эмитентов. ");
    return;
  }

  model->clear();
  emit logging("Отображение эмитентов. ");
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

  emit logging("Инициализация транспортных мастер ключей. ");
  if (Administrator->initTransportMasterKeysTable()) {
    CurrentState = Failed;
    finishOperationExecution(
        "initTransportMasterKeys",
        "Получена ошибка при инициализации транспортных мастер ключей. ");
    return;
  }

  model->clear();
  emit logging("Отображение транспортных мастер ключей. ");
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

  emit logging(QString("Связывание эмитента %1 с мастер ключами %2. ")
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
  emit logging("Отображение таблицы эмитентов. ");
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

void AdminManager::loadSettings() {}

void AdminManager::createAdministrator() {
  Administrator = new AdministrationSystem(this);
  connect(Administrator, &AdministrationSystem::logging, this,
          &AdminManager::proxyLogging);
}

void AdminManager::createTimers() {
  // Таймер, отслеживающий длительность выполняющихся операций
  ODTimer = new QTimer(this);
  ODTimer->setInterval(SERVER_MANAGER_OPERATION_MAX_DURATION);
  connect(ODTimer, &QTimer::timeout, this,
          &AdminManager::on_ODTimerTimeout_slot);
  connect(ODTimer, &QTimer::timeout, ODTimer, &QTimer::stop);
  connect(this, &AdminManager::operationPerformingFinished, ODTimer,
          &QTimer::stop);

  // Таймер для измерения длительности операции
  ODMeter = new QElapsedTimer();
}

void AdminManager::setupODQTimer(uint32_t msecs) {
  // Таймер, отслеживающий квант длительности операции
  ODQTimer = new QTimer(this);
  ODQTimer->setInterval(msecs);

  connect(ODQTimer, &QTimer::timeout, this,
          &AdminManager::on_ODQTimerTimeout_slot);
  connect(this, &AdminManager::operationPerformingFinished, ODQTimer,
          &QTimer::stop);
}

bool AdminManager::startOperationExecution(const QString& operationName) {
  // Проверяем готовность к выполнению операции
  if (CurrentState != Ready) {
    emit logging(
        "Предыдущая операция не завершена. Выполнение новой невозможно. ");
    return false;
  }

  // Переходим в состояние ожидания конца обработки
  CurrentState = WaitingExecution;

  //  Настраиваем и запускаем таймер для измерения квантов времени
  QSettings settings;
  uint64_t operationDuration = settings
                                   .value(QString("AdminManager/Operations/") +
                                          operationName + QString("/Duration"))
                                   .toInt();
  uint32_t operationQuantDuration = operationDuration / 100;
  operationQuantDuration += 10;
  emit logging(QString("Длительность кванта операции: %1.")
                   .arg(QString::number(operationQuantDuration)));
  setupODQTimer(operationQuantDuration);
  ODQTimer->start();

  // Запускаем таймер для контроля максимальной длительности операции
  ODTimer->start();

  // Запускаем измеритель длительности операции
  ODMeter->start();

  // Отправляем сигнал о начале выполнения длительной операции
  emit operationPerfomingStarted();

  return true;
}

void AdminManager::finishOperationExecution(const QString& operationName,
                                            const QString& msg) {
  QSettings settings;
  emit logging(msg);

  // Сигнал о завершении текущей операции
  emit operationPerformingFinished();

  // Оповещаем пользователя о результатах
  if (CurrentState == Completed) {
    // Измеряем и сохраняем длительность операции
    uint64_t duration = ODMeter->elapsed();
    emit logging(
        QString("Длительность операции: %1.").arg(QString::number(duration)));
    settings.setValue(QString("AdminManager/Operations/") + operationName +
                          QString("/Duration"),
                      QVariant::fromValue(duration));
    emit notifyUser(msg);
  } else {
    emit notifyUserAboutError(msg);
  }

  // Готовы к следующей операции
  CurrentState = Ready;
}

/*
 * Приватные слоты
 */

void AdminManager::proxyLogging(const QString& log) {
  if (sender()->objectName() == QString("PersoHost"))
    emit logging(QString("Host - ") + log);
  else if (sender()->objectName() == QString("AdministrationSystem"))
    emit logging(QString("Administrator - ") + log);
  else
    emit logging(QString("Unknown - ") + log);
}

void AdminManager::on_ODTimerTimeout_slot() {
  emit logging("Операция выполняется слишком долго. Сброс. ");
  emit notifyUserAboutError("Операция выполняется слишком долго. Сброс. ");
}

void AdminManager::on_ODQTimerTimeout_slot() {
  emit operationStepPerfomed();
}

//==================================================================================
