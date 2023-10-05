#include "admin_manager.h"

/* Диспетчер DSRC  */
//==================================================================================
AdminManager::AdminManager(QObject* parent) : QObject(parent) {
  setObjectName("AdminManager");

  // Создаем среду выполнения для инициализатора
  createAdministratorInstance();

  // Создаем таймеры
  createTimers();

  // Создаем цикл ожидания
  createWaitingLoop();

  // Готовы к выполнению операций
  CurrentState = Ready;
}

AdminManager::~AdminManager() {
  AdministratorThread->quit();
  AdministratorThread->wait();
}

void AdminManager::applySettings() {
  emit logging("Применение новых настроек. ");

  // Посылаем общий сигнал для применения настроек
  emit applySettings_signal();
}

void AdminManager::performDatabaseConnecting() {
  // Начинаем выполнение операции
  if (!startOperationExecution("performDatabaseConnecting")) {
    return;
  }

  emit logging("Прямое подключение к базе данных. ");
  emit connectDatabase_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("performDatabaseConnecting");
}

void AdminManager::performDatabaseDisconnecting() {
  // Начинаем выполнение операции
  if (!startOperationExecution("performDatabaseDisconnecting")) {
    return;
  }

  emit logging("Прямое отключение от базы данных. ");
  emit disconnectDatabase_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("performDatabaseDisconnecting");
}

void AdminManager::showDatabaseTable(const QString& name,
                                     DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("showDatabaseTable")) {
    return;
  }

  model->clear();
  emit logging("Отображение таблицы базы данных. ");
  emit getDatabaseTable_signal(name, model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("showDatabaseTable");
}

void AdminManager::clearDatabaseTable(const QString& name,
                                      DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("clearDatabaseTable")) {
    return;
  }

  emit logging("Очистка таблицы базы данных. ");
  emit clearDatabaseTable_signal(name);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  if (CurrentState != Completed) {
    // Завершаем выполнение операции
    endOperationExecution("clearDatabaseTable");
    return;
  }

  model->clear();
  emit logging("Отображение таблицы базы данных. ");
  emit getDatabaseTable_signal(name, model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("clearDatabaseTable");
}

void AdminManager::performCustomRequest(const QString& req,
                                        DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("performCustomRequest")) {
    return;
  }

  model->clear();
  emit logging("Представление ответа на кастомный запрос. ");
  emit getCustomResponse_signal(req, model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("performCustomRequest");
}

void AdminManager::createNewOrder(const QMap<QString, QString>* orderParameters,
                                  DatabaseTableModel* model) {
  if (!startOperationExecution("createNewOrder")) {
    return;
  }

  emit logging("Создание нового заказа. ");
  emit createNewOrder_signal(orderParameters);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("createNewOrder");
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  emit getDatabaseTable_signal("orders", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("createNewOrder");
}

void AdminManager::startOrderAssemblingManually(const QString& orderId,
                                                DatabaseTableModel* model) {
  if (!startOperationExecution("startOrderAssemblingManually")) {
    return;
  }

  emit logging(QString("Запуск сборки заказа %1. ").arg(orderId));
  emit startOrderAssembling_signal(orderId);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("startOrderAssemblingManually");
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  emit getDatabaseTable_signal("orders", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("startOrderAssemblingManually");
}

void AdminManager::stopOrderAssemblingManually(const QString& orderId,
                                               DatabaseTableModel* model) {
  if (!startOperationExecution("startOrderAssemblingManually")) {
    return;
  }

  emit logging(QString("Остановка сборки заказа %1. ").arg(orderId));
  emit stopOrderAssembling_signal(orderId);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("startOrderAssemblingManually");
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  emit getDatabaseTable_signal("orders", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("startOrderAssemblingManually");
}

void AdminManager::deleteLastOrder(DatabaseTableModel* model) {
  if (!startOperationExecution("createNewOrder")) {
    return;
  }

  emit logging("Удаление последнего заказа. ");
  emit deleteLastOrder_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("createNewOrder");
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  emit getDatabaseTable_signal("orders", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("createNewOrder");
}

void AdminManager::showOrderTable(DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("showOrderTable")) {
    return;
  }

  model->clear();
  emit logging("Отображение заказов. ");
  emit getDatabaseTable_signal("orders", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("showOrderTable");
}

void AdminManager::createNewProductionLine(
    const QMap<QString, QString>* productionLineParameters,
    DatabaseTableModel* model) {
  if (!startOperationExecution("createNewProductionLine")) {
    return;
  }

  emit logging("Создание новой линии производства. ");
  emit createNewProductionLine_signal(productionLineParameters);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("createNewProductionLine");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("createNewProductionLine");
}

void AdminManager::allocateInactiveProductionLinesManually(
    const QString& orderId,
    DatabaseTableModel* model) {
  if (!startOperationExecution("allocateInactiveProductionLinesManually")) {
    return;
  }

  emit logging(
      QString("Распределение неактивных линий производства в заказе %1. ")
          .arg(orderId));
  emit allocateInactiveProductionLines_signal(orderId);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("allocateInactiveProductionLinesManually");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("allocateInactiveProductionLinesManually");
}

void AdminManager::shutdownAllProductionLinesManually(
    DatabaseTableModel* model) {
  if (!startOperationExecution("shutdownAllProductionLinesManually")) {
    return;
  }

  emit logging(QString("Остановка всех производственных линий. "));
  emit shutdownAllProductionLines_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("shutdownAllProductionLinesManually");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("shutdownAllProductionLinesManually");
}

void AdminManager::deleteLastProductionLine(DatabaseTableModel* model) {
  if (!startOperationExecution("deleteLastProductionLine")) {
    return;
  }

  emit logging("Удаление последней линии производства. ");
  emit removeLastProductionLine_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("deleteLastProductionLine");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("deleteLastProductionLine");
}

void AdminManager::showProductionLineTable(DatabaseTableModel* model) {
  // Начинаем выполнение операции
  if (!startOperationExecution("showProductionLineTable")) {
    return;
  }

  model->clear();
  emit logging("Отображение линий производства. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("showProductionLineTable");
}

void AdminManager::linkProductionLineWithBoxManually(
    const QMap<QString, QString>* parameters,
    DatabaseTableModel* model) {
  if (!startOperationExecution("linkProductionLineWithBoxManually")) {
    return;
  }

  emit logging("Связывание линии производства с определенным боксом. ");
  emit linkProductionLineWithBox_signal(parameters);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Проверка состояния
  if (CurrentState == Failed) {
    // Завершаем выполнение операции
    endOperationExecution("linkProductionLineWithBoxManually");
    return;
  }

  model->clear();
  emit logging("Отображение производственных линий. ");
  emit getDatabaseTable_signal("production_lines", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("linkProductionLineWithBoxManually");
}

void AdminManager::initIssuers(DatabaseTableModel* model) {
  if (!startOperationExecution("initIssuers")) {
    return;
  }

  emit logging("Инициализация данных об эмитентах. ");
  emit initIssuerTable_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  if (CurrentState != Completed) {
    // Завершаем выполнение операции
    endOperationExecution("initIssuers");
    return;
  }

  model->clear();
  emit logging("Отображение эмитентов. ");
  emit getDatabaseTable_signal("issuers", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("initIssuers");
}

void AdminManager::initTransportMasterKeys(DatabaseTableModel* model) {
  if (!startOperationExecution("initTransportMasterKeys")) {
    return;
  }

  emit logging("Инициализация транспортных мастер ключей. ");
  emit initTransportMasterKeysTable_signal();

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  if (CurrentState != Completed) {
    // Завершаем выполнение операции
    endOperationExecution("initTransportMasterKeys");
    return;
  }

  model->clear();
  emit logging("Отображение транспортных мастер ключей. ");
  emit getDatabaseTable_signal("transport_master_keys", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("initTransportMasterKeys");
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
  emit linkIssuerWithMasterKeys_signal(parameters);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  if (CurrentState != Completed) {
    // Завершаем выполнение операции
    endOperationExecution("linkIssuerWithMasterKeys");
    return;
  }

  model->clear();
  emit logging("Отображение эмитентов. ");
  emit getDatabaseTable_signal("issuers", model);

  // Запускаем цикл ожидания
  WaitingLoop->exec();

  // Завершаем выполнение операции
  endOperationExecution("linkIssuerWithMasterKeys");
}

void AdminManager::createAdministratorInstance() {
  // Создаем строитель и поток для создателя отчетов
  AdministratorBuilder = new AdministrationSystemBuilder();
  AdministratorThread = new QThread(this);

  // Переносим инициализатор в поток
  AdministratorBuilder->moveToThread(AdministratorThread);

  // Когда поток завершит работу, он будет удален
  connect(AdministratorThread, &QThread::finished, AdministratorThread,
          &QObject::deleteLater);
  // Когда поток завершит работу, вызываем метод обработки
  connect(AdministratorThread, &QThread::finished, this,
          &AdminManager::on_AdministratorThreadFinished_slot);
  // Когда поток завершит работу, строитель будет удален
  connect(AdministratorThread, &QThread::finished, AdministratorBuilder,
          &QObject::deleteLater);
  // Когда поток начнет свою работу, строитель создаст в нем составитель отчетов
  connect(AdministratorThread, &QThread::started, AdministratorBuilder,
          &AdministrationSystemBuilder::build);

  // Когда строитель завершит работу, возвращем его в менеджер
  connect(AdministratorBuilder, &AdministrationSystemBuilder::completed, this,
          &AdminManager::on_AdministratorBuilderCompleted_slot);

  // Запускаем поток инициализатора
  AdministratorThread->start();
}

void AdminManager::createWaitingLoop() {
  WaitingLoop = new QEventLoop(this);
  connect(this, &AdminManager::waitingEnd, WaitingLoop, &QEventLoop::quit);
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
  if (CurrentState != Ready)
    return false;

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

void AdminManager::endOperationExecution(const QString& operationName) {
  QSettings settings;

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

    emit notifyUser(NotificarionText);
  } else {
    emit notifyUserAboutError(NotificarionText);
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

void AdminManager::on_AdministratorBuilderCompleted_slot() {
  Administrator = AdministratorBuilder->buildedObject();

  // Когда поток завершит работу, составитель заказов будет удален
  connect(AdministratorThread, &QThread::finished, Administrator,
          &QObject::deleteLater);
  // Подключаем логгирование к инициализатору
  connect(Administrator, &AdministrationSystem::logging, this,
          &AdminManager::proxyLogging);
  // Подключаем сигнал для применения новых настроек
  connect(this, &AdminManager::applySettings_signal, Administrator,
          &AdministrationSystem::applySettings);
  // После выполнения операции формирователем заказов, оповещаем менеджер
  connect(Administrator, &AdministrationSystem::operationFinished, this,
          &AdminManager::on_AdministratorFinished_slot);

  // Подключаем функционал
  connect(this, &AdminManager::connectDatabase_signal, Administrator,
          &AdministrationSystem::connectDatabase);
  connect(this, &AdminManager::disconnectDatabase_signal, Administrator,
          &AdministrationSystem::disconnectDatabase);
  connect(this, &AdminManager::getDatabaseTable_signal, Administrator,
          &AdministrationSystem::getDatabaseTable);
  connect(this, &AdminManager::getCustomResponse_signal, Administrator,
          &AdministrationSystem::getCustomResponse);
  connect(this, &AdminManager::clearDatabaseTable_signal, Administrator,
          &AdministrationSystem::clearDatabaseTable);

  connect(this, &AdminManager::createNewOrder_signal, Administrator,
          &AdministrationSystem::createNewOrder);
  connect(this, &AdminManager::startOrderAssembling_signal, Administrator,
          &AdministrationSystem::startOrderAssembling);
  connect(this, &AdminManager::stopOrderAssembling_signal, Administrator,
          &AdministrationSystem::stopOrderAssembling);
  connect(this, &AdminManager::deleteLastOrder_signal, Administrator,
          &AdministrationSystem::deleteLastOrder);

  connect(this, &AdminManager::createNewProductionLine_signal, Administrator,
          &AdministrationSystem::createNewProductionLine);
  connect(this, &AdminManager::allocateInactiveProductionLines_signal,
          Administrator,
          &AdministrationSystem::allocateInactiveProductionLines);
  connect(this, &AdminManager::shutdownAllProductionLines_signal, Administrator,
          &AdministrationSystem::shutdownAllProductionLines);
  connect(this, &AdminManager::removeLastProductionLine_signal, Administrator,
          &AdministrationSystem::deleteLastProductionLine);
  connect(this, &AdminManager::linkProductionLineWithBox_signal, Administrator,
          &AdministrationSystem::linkProductionLineWithBox);

  connect(this, &AdminManager::initIssuerTable_signal, Administrator,
          &AdministrationSystem::initIssuerTable);
  connect(this, &AdminManager::initTransportMasterKeysTable_signal,
          Administrator, &AdministrationSystem::initTransportMasterKeysTable);
  connect(this, &AdminManager::linkIssuerWithMasterKeys_signal, Administrator,
          &AdministrationSystem::linkIssuerWithMasterKeys);
}

void AdminManager::on_AdministratorThreadFinished_slot() {
  emit logging("Поток инициализатора завершился. ");
  Administrator = nullptr;
  AdministratorThread = nullptr;
}

void AdminManager::on_AdministratorFinished_slot(
    AdministrationSystem::ExecutionStatus status) {
  switch (status) {
    case AdministrationSystem::NotExecuted:
      CurrentState = Failed;
      NotificarionText = "Администратор: операция не была запущена. ";
      break;
    case AdministrationSystem::DatabaseConnectionError:
      CurrentState = Failed;
      NotificarionText =
          "Администратор: не удалось подключиться к базе данных. ";
      break;
    case AdministrationSystem::DatabaseQueryError:
      CurrentState = Failed;
      NotificarionText =
          "Администратор: ошибка при выполнении запроса к базе данных. ";
      break;
    case AdministrationSystem::LogicError:
      CurrentState = Failed;
      NotificarionText = "Администратор: получена логическая ошибка. ";
      break;
    case AdministrationSystem::ReleaserError:
      CurrentState = Failed;
      NotificarionText =
          "Администратор: получена ошибка в системе выпуска "
          "транспондеров. ";
      break;
    case AdministrationSystem::UnknowError:
      CurrentState = Failed;
      NotificarionText =
          "Администратор: получена неизвестная ошибка при выполнении "
          "операции. ";
      break;
    case AdministrationSystem::CompletedSuccessfully:
      CurrentState = Completed;
      NotificarionText = "Операция успешно выполнена. ";
      break;
  }

  // Выходим из цикла ожидания
  emit waitingEnd();
}

void AdminManager::on_ODTimerTimeout_slot() {
  emit logging("Операция выполняется слишком долго. Сброс. ");
  emit notifyUserAboutError("Операция выполняется слишком долго. Сброс. ");

  // Выходим из цикла ожидания
  emit waitingEnd();
}

void AdminManager::on_ODQTimerTimeout_slot() {
  emit operationStepPerfomed();
}

//==================================================================================
