#include "definitions.h"
#include "progress_indicator.h"

ProgressIndicator::ProgressIndicator(const QString& name)
    : AbstractProgressIndicator{name}, LoggableObject(name) {
  createTimers();
}

ProgressIndicator::~ProgressIndicator() {}

void ProgressIndicator::begin(const QString& operationName) {
  sendLog(QString("Начало выполнения операции '%1'. ").arg(operationName));

  // Создаем  окно
  createProgressDialog();

  //  Настраиваем и запускаем таймер для измерения квантов времени
  uint64_t operationDuration =
      Settings.value(QString("duration_of_operations/") + operationName)
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

void ProgressIndicator::finish(const QString& operationName) {
  sendLog(QString("Выполнение операции '%1' завершено. ").arg(operationName));

  // Измеряем и сохраняем длительность операции
  uint64_t duration = ODMeter->elapsed();
  sendLog(QString("Длительность операции: %1.").arg(QString::number(duration)));
  Settings.setValue(QString("duration_of_operations/") + operationName,
                    QVariant::fromValue(duration));

  // Останавливаем таймер для контроля максимальной длительности операции
  ODTimer->stop();

  // Останавливаем для измерения кванта длительности операции
  ODQTimer->stop();

  // Закрываем окно
  destroyProgressDialog();
}

/*
 * Приватные методы
 */

void ProgressIndicator::createProgressDialog() {
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

void ProgressIndicator::destroyProgressDialog() {
  ProgressDialog->close();
}

void ProgressIndicator::createTimers() {
  // Таймер, отслеживающий длительность выполняющихся операций
  ODTimer = std::unique_ptr<QTimer>(new QTimer());
  ODTimer->setInterval(SERVER_MANAGER_OPERATION_MAX_DURATION);
  connect(ODTimer.get(), &QTimer::timeout, this,
          &ProgressIndicator::ODTimerTimeout_slot);
  connect(ODTimer.get(), &QTimer::timeout, ODTimer.get(), &QTimer::stop);

  // Таймер для измерения длительности операции
  ODMeter = std::unique_ptr<QElapsedTimer>(new QElapsedTimer());

  // Таймер, отслеживающий квант длительности операции
  ODQTimer = std::unique_ptr<QTimer>(new QTimer());
  connect(ODQTimer.get(), &QTimer::timeout, this,
          &ProgressIndicator::ODQTimerTimeout_slot);
}

void ProgressIndicator::progressDialogCanceled_slot() {
  ProgressDialog->close();

  emit abortCurrentOperation();
}

void ProgressIndicator::ODTimerTimeout_slot() {
  sendLog("Операция выполняется слишком долго. Сброс. ");
}

void ProgressIndicator::ODQTimerTimeout_slot() {
  int32_t cvalue = ProgressDialog->value();

  if (cvalue < 99) {
    ProgressDialog->setValue(++cvalue);
  }
}
