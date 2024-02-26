#include "perso_server_gui_subkernel.h"
#include "authorization_dialog.h"
#include "global_environment.h"
#include "numeric_string_checker.h"
#include "perso_server_async_wrapper.h"
#include "string_input_dialog.h"

PersoServerGuiSubkernel::PersoServerGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name), FirmwareDisplay(nullptr) {
  createModels();
  connectDependecies();
}

PersoServerGuiSubkernel::~PersoServerGuiSubkernel() {}

HashTableModel& PersoServerGuiSubkernel::productionLine() {
  return ProductionLine;
}

HashTableModel& PersoServerGuiSubkernel::box() {
  return Box;
}

HashTableModel& PersoServerGuiSubkernel::transponder() {
  return Transponder;
}

void PersoServerGuiSubkernel::setFirmwareDisplay(
    QPlainTextEdit* firmwareDisplay) {
  FirmwareDisplay = firmwareDisplay;
}

void PersoServerGuiSubkernel::connect() {
  emit clearLogDisplay();
  emit connect_signal();
}

void PersoServerGuiSubkernel::disconnect() {
  emit clearLogDisplay();
  emit disconnect_signal();
}

void PersoServerGuiSubkernel::echo() {
  emit clearLogDisplay();
  emit echo_signal();
}

void PersoServerGuiSubkernel::logOn() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit logOn_signal(param);
}

void PersoServerGuiSubkernel::logOut() {
  emit clearLogDisplay();
  emit logOut_signal();
}

void PersoServerGuiSubkernel::launchProductionLine() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit launchProductionLine_signal(param);
}

void PersoServerGuiSubkernel::shutdownProductionLine() {
  emit clearLogDisplay();
  emit shutdownProductionLine_signal();
}

void PersoServerGuiSubkernel::getProductionLineData() {
  emit clearLogDisplay();
  emit getProductionLineData_signal();
}

void PersoServerGuiSubkernel::requestBox() {
  emit clearLogDisplay();
  emit requestBox_signal();
}

void PersoServerGuiSubkernel::getCurrentBoxData() {
  emit clearLogDisplay();
  emit getCurrentBoxData_signal();
}

void PersoServerGuiSubkernel::refundCurrentBox() {
  emit clearLogDisplay();
  emit refundCurrentBox_signal();
}

void PersoServerGuiSubkernel::completeCurrentBox() {
  emit clearLogDisplay();
  emit completeCurrentBox_signal();
}

void PersoServerGuiSubkernel::releaseTransponder() {
  emit clearLogDisplay();
  emit releaseTransponder_signal();
}

void PersoServerGuiSubkernel::rereleaseTransponder() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog;
  NumericStringChecker checker;
  dialog.setChecker(&checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit rereleaseTransponder_signal(param);
}

void PersoServerGuiSubkernel::rollbackTransponder() {
  emit clearLogDisplay();
  emit rollbackTransponder_signal();
}

void PersoServerGuiSubkernel::getCurrentTransponderData() {
  emit clearLogDisplay();
  emit getCurrentTransponderData_signal();
}

void PersoServerGuiSubkernel::getTransponderData() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog;
  NumericStringChecker checker;
  dialog.setChecker(&checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit getTransponderData_signal(param);
}

void PersoServerGuiSubkernel::printBoxSticker() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printBoxSticker_signal(param);
}

void PersoServerGuiSubkernel::printLastBoxSticker() {
  emit clearLogDisplay();
  emit printLastBoxSticker_signal();
}

void PersoServerGuiSubkernel::printPalletSticker() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printPalletSticker_signal(param);
}

void PersoServerGuiSubkernel::printLastPalletSticker() {
  emit clearLogDisplay();
  emit printLastPalletSticker_signal();
}

void PersoServerGuiSubkernel::displayProductionLineData(
    const std::shared_ptr<StringDictionary> data) {
  ProductionLine.setData(data);
}

void PersoServerGuiSubkernel::displayBoxData(
    const std::shared_ptr<StringDictionary> data) {
  Box.setData(data);
}

void PersoServerGuiSubkernel::displayTransponderData(
    const std::shared_ptr<StringDictionary> data) {
  Transponder.setData(data);
}

void PersoServerGuiSubkernel::displayFirmware(
    const std::shared_ptr<QByteArray> firmware) {
  if (!FirmwareDisplay) {
    return;
  }

  FirmwareDisplay->setPlainText(firmware->toHex());
}

void PersoServerGuiSubkernel::connectDependecies() {
  const PersoServerManager* psm = static_cast<const PersoServerManager*>(
      GlobalEnvironment::instance()->getObject("PersoServerManager"));

  // К менеджерам
  QObject::connect(this, &PersoServerGuiSubkernel::connect_signal, psm,
                   &PersoServerManager::connect);
  QObject::connect(this, &PersoServerGuiSubkernel::disconnect_signal, psm,
                   &PersoServerManager::disconnect);

  QObject::connect(this, &PersoServerGuiSubkernel::echo_signal, psm,
                   &PersoServerManager::echo);
  QObject::connect(this, &PersoServerGuiSubkernel::logOn_signal, psm,
                   &PersoServerManager::logOn);
  QObject::connect(this, &PersoServerGuiSubkernel::logOut_signal, psm,
                   &PersoServerManager::logOut);

  QObject::connect(this, &PersoServerGuiSubkernel::launchProductionLine_signal,
                   psm, &PersoServerManager::launchProductionLine);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::shutdownProductionLine_signal, psm,
                   &PersoServerManager::shutdownProductionLine);
  QObject::connect(this, &PersoServerGuiSubkernel::getProductionLineData_signal,
                   psm, &PersoServerManager::getProductionLineData);

  QObject::connect(this, &PersoServerGuiSubkernel::requestBox_signal, psm,
                   &PersoServerManager::requestBox);
  QObject::connect(this, &PersoServerGuiSubkernel::getCurrentBoxData_signal,
                   psm, &PersoServerManager::getCurrentBoxData);
  QObject::connect(this, &PersoServerGuiSubkernel::refundCurrentBox_signal, psm,
                   &PersoServerManager::refundCurrentBox);
  QObject::connect(this, &PersoServerGuiSubkernel::completeCurrentBox_signal,
                   psm, &PersoServerManager::completeCurrentBox);

  QObject::connect(this, &PersoServerGuiSubkernel::releaseTransponder_signal,
                   psm, &PersoServerManager::releaseTransponder);
  QObject::connect(this, &PersoServerGuiSubkernel::rereleaseTransponder_signal,
                   psm, &PersoServerManager::rereleaseTransponder);
  QObject::connect(this, &PersoServerGuiSubkernel::rollbackTransponder_signal,
                   psm, &PersoServerManager::rollbackTransponder);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::getCurrentTransponderData_signal,
                   psm, &PersoServerManager::getCurrentTransponderData);
  QObject::connect(this, &PersoServerGuiSubkernel::getTransponderData_signal,
                   psm, &PersoServerManager::getTransponderData);

  QObject::connect(this, &PersoServerGuiSubkernel::printBoxSticker_signal, psm,
                   &PersoServerManager::printBoxSticker);
  QObject::connect(this, &PersoServerGuiSubkernel::printLastBoxSticker_signal,
                   psm, &PersoServerManager::printLastBoxSticker);
  QObject::connect(this, &PersoServerGuiSubkernel::printPalletSticker_signal,
                   psm, &PersoServerManager::printPalletSticker);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::printLastPalletSticker_signal, psm,
                   &PersoServerManager::printLastPalletSticker);

  // От менеджеров
  QObject::connect(psm, &PersoServerManager::productionLineDataReady, this,
                   &PersoServerGuiSubkernel::displayProductionLineData);
  QObject::connect(psm, &PersoServerManager::boxDataReady, this,
                   &PersoServerGuiSubkernel::displayBoxData);
  QObject::connect(psm, &PersoServerManager::transponderDataReady, this,
                   &PersoServerGuiSubkernel::displayTransponderData);

  QObject::connect(psm, &PersoServerManager::transponderDataReady, this,
                   &PersoServerGuiSubkernel::displayTransponderData);
}

void PersoServerGuiSubkernel::createModels() {
  std::shared_ptr<StringDictionary> plMatchTable(new StringDictionary());
  plMatchTable->insert("production_line_id", "Идентификатор");
  plMatchTable->insert("production_line_login", "Логин");
  plMatchTable->insert("production_line_in_process", "В процессе сборки");
  plMatchTable->insert("production_line_ns", "Сотрудник");
  plMatchTable->insert("today_assembled_boxes", "Собрано боксов за сегодня");
  plMatchTable->insert("transponder_id", "Идентификатор транспондера");
  plMatchTable->insert("box_id", "Идентификатор бокса");

  ProductionLine.setMatchTable(plMatchTable);

  std::shared_ptr<StringDictionary> tMatchTable(new StringDictionary());
  tMatchTable->insert("transponder_sn", "Серийный номер");
  tMatchTable->insert("transponder_ucid", "UCID");
  tMatchTable->insert("transponder_pan", "PAN");
  tMatchTable->insert("box_id", "Идентификатор бокса");
  tMatchTable->insert("transponder_release_counter", "Количество выпусков");
  tMatchTable->insert("issuer_name", "Заказчик");

  Transponder.setMatchTable(tMatchTable);

  std::shared_ptr<StringDictionary> bMatchTable(new StringDictionary());
  bMatchTable = std::shared_ptr<StringDictionary>(new StringDictionary());
  bMatchTable->insert("box_id", "ID бокса");
  bMatchTable->insert("box_in_process", "В процессе");
  bMatchTable->insert("box_quantity", "Емкость бокса");

  bMatchTable->insert("box_assembled_units", "Собрано транспондеров");
  bMatchTable->insert("box_assembling_start", "Начало сборки");
  bMatchTable->insert("box_assembling_end", "Конец сборки");

  bMatchTable->insert("first_transponder_sn", "Первый транспондер");
  bMatchTable->insert("last_transponder_sn", "Последний транспондер");

  bMatchTable->insert("pallet_id", "ID паллеты");
  bMatchTable->insert("production_line_id", "ID производственной линии");

  Box.setMatchTable(bMatchTable);
}
