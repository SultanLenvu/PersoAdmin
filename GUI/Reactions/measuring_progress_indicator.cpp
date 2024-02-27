#include "definitions.h"
#include "measuring_progress_indicator.h"

MeasuringProgressIndicator::MeasuringProgressIndicator(const QString& name)
    : AbstractProgressIndicator{name} {
  createTimers();
}

MeasuringProgressIndicator::~MeasuringProgressIndicator() {}

void MeasuringProgressIndicator::begin(const QString& operationName) {
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

void MeasuringProgressIndicator::finish(const QString& operationName) {
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
}

/*
 * Приватные методы
 */

void MeasuringProgressIndicator::createProgressDialog() {
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

void MeasuringProgressIndicator::destroyProgressDialog() {
  ProgressDialog->close();
}

void MeasuringProgressIndicator::createTimers() {
  // Таймер, отслеживающий длительность выполняющихся операций
  ODTimer = std::unique_ptr<QTimer>(new QTimer());
  ODTimer->setInterval(SERVER_MANAGER_OPERATION_MAX_DURATION);
  connect(ODTimer.get(), &QTimer::timeout, this,
          &MeasuringProgressIndicator::ODTimerTimeout_slot);
  connect(ODTimer.get(), &QTimer::timeout, ODTimer.get(), &QTimer::stop);

  // Таймер для измерения длительности операции
  ODMeter = std::unique_ptr<QElapsedTimer>(new QElapsedTimer());

  // Таймер, отслеживающий квант длительности операции
  ODQTimer = std::unique_ptr<QTimer>(new QTimer());
  connect(ODQTimer.get(), &QTimer::timeout, this,
          &MeasuringProgressIndicator::ODQTimerTimeout_slot);
}

void MeasuringProgressIndicator::progressDialogCanceled_slot() {
  ProgressDialog->close();

  emit abortCurrentOperation();
}

void MeasuringProgressIndicator::ODTimerTimeout_slot() {
  sendLog("Операция выполняется слишком долго. Сброс. ");
}

void MeasuringProgressIndicator::ODQTimerTimeout_slot() {
  int32_t cvalue = ProgressDialog->value();

  if (cvalue < 99) {
    ProgressDialog->setValue(++cvalue);
  }
}
