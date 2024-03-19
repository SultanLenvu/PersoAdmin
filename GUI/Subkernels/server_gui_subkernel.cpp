#include "server_gui_subkernel.h"
#include "authorization_dialog.h"
#include "global_environment.h"
#include "numeric_string_checker.h"
#include "server_connection_async_wrapper.h"
#include "string_input_dialog.h"

ServerGuiSubkernel::ServerGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}
void ServerGuiSubkernel::executeCommand(const QString& name) {
  emit clearLogDisplay();
}

void ServerGuiSubkernel::connect() {
  emit clearLogDisplay();
  emit connect_signal();
}

void ServerGuiSubkernel::disconnect() {
  emit clearLogDisplay();
  emit disconnect_signal();
}

void ServerGuiSubkernel::echo() {
  emit clearLogDisplay();
  emit echo_signal();
}

void ServerGuiSubkernel::launchProductionLine() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit launchProductionLine_signal(param);
}

void ServerGuiSubkernel::shutdownProductionLine() {
  emit clearLogDisplay();
  emit shutdownProductionLine_signal();
}

void ServerGuiSubkernel::getProductionLineData() {
  emit clearLogDisplay();
  emit getProductionLineData_signal();
}

void ServerGuiSubkernel::requestBox() {
  emit clearLogDisplay();
  emit requestBox_signal();
}

void ServerGuiSubkernel::getCurrentBoxData() {
  emit clearLogDisplay();
  emit getCurrentBoxData_signal();
}

void ServerGuiSubkernel::refundCurrentBox() {
  emit clearLogDisplay();
  emit refundCurrentBox_signal();
}

void ServerGuiSubkernel::completeCurrentBox() {
  emit clearLogDisplay();
  emit completeCurrentBox_signal();
}

void ServerGuiSubkernel::releaseTransponder() {
  emit clearLogDisplay();
  emit releaseTransponder_signal();
}

void ServerGuiSubkernel::rereleaseTransponder() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog("transponder_pan");
  NumericStringChecker checker;
  dialog.setChecker(&checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit rereleaseTransponder_signal(param);
}

void ServerGuiSubkernel::rollbackTransponder() {
  emit clearLogDisplay();
  emit rollbackTransponder_signal();
}

void ServerGuiSubkernel::getCurrentTransponderData() {
  emit clearLogDisplay();
  emit getCurrentTransponderData_signal();
}

void ServerGuiSubkernel::getTransponderData() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog("transponder_pan");
  NumericStringChecker checker;
  dialog.setChecker(&checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit getTransponderData_signal(param);
}

void ServerGuiSubkernel::printBoxSticker() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog("transponder_pan");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printBoxSticker_signal(param);
}

void ServerGuiSubkernel::printLastBoxSticker() {
  emit clearLogDisplay();
  emit printLastBoxSticker_signal();
}

void ServerGuiSubkernel::printPalletSticker() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog("transponder_pan");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

  emit clearLogDisplay();
  emit printPalletSticker_signal(param);
}

void ServerGuiSubkernel::printLastPalletSticker() {
  emit clearLogDisplay();
  emit printLastPalletSticker_signal();
}

void ServerGuiSubkernel::displayProductionLineData(
    const std::shared_ptr<StringDictionary> data) {
  ProductionLine.setData(data);
}

void ServerGuiSubkernel::displayBoxData(
    const std::shared_ptr<StringDictionary> data) {
  Box.setData(data);
}

void ServerGuiSubkernel::displayTransponderData(
    const std::shared_ptr<StringDictionary> data) {
  Transponder.setData(data);
}

void ServerGuiSubkernel::displayFirmware(
    const std::shared_ptr<QByteArray> firmware) {
  if (!FirmwareDisplay) {
    return;
  }

  FirmwareDisplay->setPlainText(firmware->toHex());
}

void ServerGuiSubkernel::connectDependecies() {
  const ServerConnectionAsyncWrapper* psm =
      static_cast<const ServerConnectionAsyncWrapper*>(
          GlobalEnvironment::instance()->getObject("ServerConnectionAsyncWrapper"));

  // К менеджерам
  QObject::connect(this, &ServerGuiSubkernel::connect_signal, psm,
                   &ServerConnectionAsyncWrapper::connect);
  QObject::connect(this, &ServerGuiSubkernel::disconnect_signal, psm,
                   &ServerConnectionAsyncWrapper::disconnect);

  QObject::connect(this, &ServerGuiSubkernel::echo_signal, psm,
                   &ServerConnectionAsyncWrapper::echo);
  QObject::connect(this, &ServerGuiSubkernel::logOn_signal, psm,
                   &ServerConnectionAsyncWrapper::logOn);
  QObject::connect(this, &ServerGuiSubkernel::logOut_signal, psm,
                   &ServerConnectionAsyncWrapper::logOut);

  QObject::connect(this, &ServerGuiSubkernel::launchProductionLine_signal,
                   psm, &ServerConnectionAsyncWrapper::launchProductionLine);
  QObject::connect(this,
                   &ServerGuiSubkernel::shutdownProductionLine_signal, psm,
                   &ServerConnectionAsyncWrapper::shutdownProductionLine);
  QObject::connect(this, &ServerGuiSubkernel::getProductionLineData_signal,
                   psm, &ServerConnectionAsyncWrapper::getProductionLineData);

  QObject::connect(this, &ServerGuiSubkernel::requestBox_signal, psm,
                   &ServerConnectionAsyncWrapper::requestBox);
  QObject::connect(this, &ServerGuiSubkernel::getCurrentBoxData_signal,
                   psm, &ServerConnectionAsyncWrapper::getCurrentBoxData);
  QObject::connect(this, &ServerGuiSubkernel::refundCurrentBox_signal, psm,
                   &ServerConnectionAsyncWrapper::refundCurrentBox);
  QObject::connect(this, &ServerGuiSubkernel::completeCurrentBox_signal,
                   psm, &ServerConnectionAsyncWrapper::completeCurrentBox);

  QObject::connect(this, &ServerGuiSubkernel::releaseTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::releaseTransponder);
  QObject::connect(this, &ServerGuiSubkernel::rereleaseTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::rereleaseTransponder);
  QObject::connect(this, &ServerGuiSubkernel::rollbackTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::rollbackTransponder);
  QObject::connect(this,
                   &ServerGuiSubkernel::getCurrentTransponderData_signal,
                   psm, &ServerConnectionAsyncWrapper::getCurrentTransponderData);
  QObject::connect(this, &ServerGuiSubkernel::getTransponderData_signal,
                   psm, &ServerConnectionAsyncWrapper::getTransponderData);

  QObject::connect(this, &ServerGuiSubkernel::printBoxSticker_signal, psm,
                   &ServerConnectionAsyncWrapper::printBoxSticker);
  QObject::connect(this, &ServerGuiSubkernel::printLastBoxSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printLastBoxSticker);
  QObject::connect(this, &ServerGuiSubkernel::printPalletSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printPalletSticker);
  QObject::connect(this,
                   &ServerGuiSubkernel::printLastPalletSticker_signal, psm,
                   &ServerConnectionAsyncWrapper::printLastPalletSticker);

  // От менеджеров
  QObject::connect(psm, &ServerConnectionAsyncWrapper::productionLineDataReady, this,
                   &ServerGuiSubkernel::displayProductionLineData);
  QObject::connect(psm, &ServerConnectionAsyncWrapper::boxDataReady, this,
                   &ServerGuiSubkernel::displayBoxData);
  QObject::connect(psm, &ServerConnectionAsyncWrapper::transponderDataReady, this,
                   &ServerGuiSubkernel::displayTransponderData);

  QObject::connect(psm, &ServerConnectionAsyncWrapper::transponderDataReady, this,
                   &ServerGuiSubkernel::displayTransponderData);
}

void ServerGuiSubkernel::createModels() {
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
