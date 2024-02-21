#include "interaction_system.h"
#include "definitions.h"

InteractionSystem::InteractionSystem(const QString& name) : PObject{name} {
  createMessageMatchTable();
  createTimers();
}

InteractionSystem::~InteractionSystem() {}

void InteractionSystem::generateMessage(const QString& text) {
  QMessageBox::information(nullptr, "Сообщение", text, QMessageBox::Ok);
}

void InteractionSystem::generateErrorMessage(const QString& text) {
  QMessageBox::critical(nullptr, "Ошибка", text, QMessageBox::Ok);
}

void InteractionSystem::processOperationStart(const QString& operationName) {
  QSettings settings;

  // Создаем  окно
  createProgressDialog();

  //  Настраиваем и запускаем таймер для измерения квантов времени
  uint64_t operationDuration =
      settings.value(QString("duration_of_operations/") + operationName)
          .toInt();
  uint32_t operationQuantDuration = operationDuration / 100;
  operationQuantDuration += 1;
  sendLog(QString("Длительность кванта операции: %1.")
              .arg(QString::number(operationQuantDuration)));
  ODQTimer->setInterval(operationQuantDuration);
  ODQTimer->start();

  // Запускаем таймер для контроля максимальной длительности операции
  ODTimer->start();

  // Запускаем измеритель длительности операции
  ODMeter->start();
}

void InteractionSystem::processOperationFinish(const QString& operationName,
                                               ReturnStatus ret) {
  QSettings settings;

  // Измеряем и сохраняем длительность операции
  uint64_t duration = ODMeter->elapsed();
  sendLog(QString("Длительность операции: %1.").arg(QString::number(duration)));
  settings.setValue(QString("duration_of_operations/") + operationName,
                    QVariant::fromValue(duration));

  // Останавливаем таймер для контроля максимальной длительности операции
  ODTimer->stop();

  // Останавливаем для измерения кванта длительности операции
  ODQTimer->stop();

  // Закрываем окно
  destroyProgressDialog();

  // Обрабатываем статус возврата
  processReturnStatus(ret);
}

/*
 * Приватные методы
 */

void InteractionSystem::createProgressDialog() {
  ProgressDialog = std::unique_ptr<QProgressDialog>(new QProgressDialog());

  ProgressDialog->setLabelText("Выполнение операции...");
  ProgressDialog->setMinimum(0);
  ProgressDialog->setMaximum(100);
  ProgressDialog->setCancelButton(nullptr);
  ProgressDialog->setWindowModality(Qt::ApplicationModal);
  ProgressDialog->setAutoClose(false);
  ProgressDialog->setValue(0);
  ProgressDialog->show();
}

void InteractionSystem::destroyProgressDialog() {
  ProgressDialog->close();
}

void InteractionSystem::createTimers() {
  // Таймер, отслеживающий длительность выполняющихся операций
  ODTimer = std::unique_ptr<QTimer>(new QTimer());
  ODTimer->setInterval(SERVER_MANAGER_OPERATION_MAX_DURATION);
  connect(ODTimer.get(), &QTimer::timeout, this,
          &InteractionSystem::ODTimerTimeout_slot);
  connect(ODTimer.get(), &QTimer::timeout, ODTimer.get(), &QTimer::stop);

  // Таймер для измерения длительности операции
  ODMeter = std::unique_ptr<QElapsedTimer>(new QElapsedTimer());

  // Таймер, отслеживающий квант длительности операции
  ODQTimer = std::unique_ptr<QTimer>(new QTimer());
  connect(ODQTimer.get(), &QTimer::timeout, this,
          &InteractionSystem::ODQTimerTimeout_slot);
}

void InteractionSystem::createMessageMatchTable() {
  // Заполняем таблицу соответствий статусов возврата
  MessageTable[ReturnStatus::NoError] = QString("Выполнено. ");

  MessageTable[ReturnStatus::ParameterError] =
      QString("Получены некорректные параметры.");
  MessageTable[ReturnStatus::DynamicLibraryMissing] =
      QString("Отсутствует библиотека для работы с принтером стикеров. ");
  MessageTable[ReturnStatus::StickerPrinterConnectionError] =
      QString("Не удалось подключиться к стикер-принтеру. ");
  MessageTable[ReturnStatus::StickerPrintingFailed] =
      QString("Не удалось распечать стикер. ");

  MessageTable[ReturnStatus::DatabaseConnectionError] =
      QString("Не удалось подключиться к базе данных. ");
  MessageTable[ReturnStatus::DatabaseTransactionError] =
      QString("Ошибка транзакции. ");
  MessageTable[ReturnStatus::DatabaseQueryError] =
      QString("Получена ошибка при выполнении запроса к базе данных.");

  MessageTable[ReturnStatus::RegisterFileError] =
      QString("Не удалось открыть файл-реестр для отгрузки.");

  MessageTable[ReturnStatus::TransponderMissed] =
      QString("Транспондер не найден.");
  MessageTable[ReturnStatus::BoxMissed] = QString("Бокс не найден.");
  MessageTable[ReturnStatus::PalletMissed] = QString("Паллета не найдена.");
  MessageTable[ReturnStatus::OrderMissed] = QString("Заказ не найден.");

  MessageTable[ReturnStatus::ProductionLineMissed] =
      QString("Не удалось найти производственную линию.");
  MessageTable[ReturnStatus::ProductionLineLinkError] =
      QString("Не удалось связать производственную линию с боксом.");
  MessageTable[ReturnStatus::ProductionLineRollbackLimit] = QString(
      "Производственная линия находится на первом транспондере в боксе.");

  MessageTable[ReturnStatus::OrderRemovingError] =
      QString("Не удалось удалить заказ.");
  MessageTable[ReturnStatus::OtherOrderInProcess] =
      QString("В процессе сборки находится другой заказ.");
  MessageTable[ReturnStatus::OrderNotInProcess] =
      QString("Сборка заказа не была запущена.");

  MessageTable[ReturnStatus::OrderMultiplyAssembly] =
      QString("В процессе сборки находится несколько заказов.");
  MessageTable[ReturnStatus::FreeBoxMissed] =
      QString("Не удалось найти свободный бокс.");
  MessageTable[ReturnStatus::UnknownError] = QString("Неизвествная ошибка. ");
}

void InteractionSystem::processReturnStatus(ReturnStatus ret) {
  if (ret == ReturnStatus::NoError) {
    QMessageBox::information(nullptr, "Сообщение", MessageTable[ret],
                             QMessageBox::Ok);
    return;
  }
  if (MessageTable.count(ret) == 0) {
    QMessageBox::critical(nullptr, "Ошибка",
                          "Получен неизвестный статус возврата.",
                          QMessageBox::Ok);
    return;
  }

  QMessageBox::critical(nullptr, "Ошибка", MessageTable[ret], QMessageBox::Ok);
}

void InteractionSystem::progressDialogCanceled_slot() {
  ProgressDialog->close();

  emit abortCurrentOperation();
}

void InteractionSystem::ODTimerTimeout_slot() {
  sendLog("Операция выполняется слишком долго. Сброс. ");
  generateErrorMessage("Операция выполняется слишком долго. Сброс. ");
}

void InteractionSystem::ODQTimerTimeout_slot() {
  int32_t cvalue = ProgressDialog->value();

  if (cvalue < 99) {
    ProgressDialog->setValue(++cvalue);
  }
}
