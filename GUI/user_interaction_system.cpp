#include "user_interaction_system.h"

UserInteractionSystem::UserInteractionSystem(QWidget* parent)
    : QWidget(parent) {
  setObjectName("UserInteractionSystem");
  loadSettings();

  ProgressDialog = nullptr;
  CurrentOperationStep = 0;

  // Создаем таймеры
  createTimers();
}

void UserInteractionSystem::generateMessage(const QString& data) {
  QMessageBox::information(this, "Сообщение", data, QMessageBox::Ok);
}

void UserInteractionSystem::getMasterPassword(QString& pass) {
  pass = QInputDialog::getText(
      this, "Мастер доступ", "Введите пароль:", QLineEdit::Normal, "", nullptr);
}

void UserInteractionSystem::generateErrorMessage(const QString& text) {
  QMessageBox::critical(this, "Ошибка", text, QMessageBox::Ok);
}

void UserInteractionSystem::startOperationProgressDialog(
    const QString& operationName) {
  QSettings settings;

  // Создаем  окно
  createProgressDialog();

  //  Настраиваем и запускаем таймер для измерения квантов времени
  uint64_t operationDuration =
      settings
          .value(QString("Operations/") + operationName + QString("/Duration"))
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

void UserInteractionSystem::finishOperationProgressDialog(
    const QString& operationName) {
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

void UserInteractionSystem::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
}

/*
 * Приватные методы
 */

void UserInteractionSystem::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("user_interaction_system/log_enable").toBool();
}

void UserInteractionSystem::sendLog(const QString& log) {
  if (LogEnable) {
    emit logging("UserInteractionSystem - " + log);
  }
}

void UserInteractionSystem::createProgressDialog() {
  ProgressDialog =
      new QProgressDialog("Выполнение операции...", "Закрыть", 0, 100);
  ProgressDialog->setWindowModality(Qt::ApplicationModal);
  ProgressDialog->setAutoClose(false);
  ProgressDialog->show();
}

void UserInteractionSystem::destroyProgressDialog() {
  if (!ProgressDialog)
    return;

  ProgressDialog->close();
  delete ProgressDialog;
  ProgressDialog = nullptr;
  CurrentOperationStep = 0;
}

void UserInteractionSystem::createTimers() {
  // Таймер, отслеживающий длительность выполняющихся операций
  ODTimer = new QTimer(this);
  ODTimer->setInterval(SERVER_MANAGER_OPERATION_MAX_DURATION);
  connect(ODTimer, &QTimer::timeout, this,
          &UserInteractionSystem::on_ODTimerTimeout_slot);
  connect(ODTimer, &QTimer::timeout, ODTimer, &QTimer::stop);

  // Таймер для измерения длительности операции
  ODMeter = new QElapsedTimer();

  // Таймер, отслеживающий квант длительности операции
  ODQTimer = new QTimer(this);
  connect(ODQTimer, &QTimer::timeout, this,
          &UserInteractionSystem::on_ODQTimerTimeout_slot);
}

void UserInteractionSystem::on_ProgressDialogCanceled_slot() {
  destroyProgressDialog();

  emit abortCurrentOperation();
}

void UserInteractionSystem::on_ODTimerTimeout_slot() {
  sendLog("Операция выполняется слишком долго. Сброс. ");
  generateErrorMessage("Операция выполняется слишком долго. Сброс. ");
}

void UserInteractionSystem::on_ODQTimerTimeout_slot() {
  if (!ProgressDialog) {
    return;
  }

  CurrentOperationStep++;
  if (CurrentOperationStep < 100) {
    ProgressDialog->setValue(CurrentOperationStep);
  }
}
