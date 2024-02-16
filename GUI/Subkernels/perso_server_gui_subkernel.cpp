#include "perso_server_gui_subkernel.h"
#include "global_environment.h"
#include "perso_server_manager.h"
#include "string_input_dialog.h"

PersoServerGuiSubkernel::PersoServerGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
  createModels();

  FirmwareDisplay = nullptr;
}

PersoServerGuiSubkernel::~PersoServerGuiSubkernel() {}

HashTableModel* PersoServerGuiSubkernel::productionLine() {
  return ProductionLine.get();
}

HashTableModel* PersoServerGuiSubkernel::box() {
  return Box.get();
}

HashTableModel* PersoServerGuiSubkernel::transponder() {
  return Transponder.get();
}

void PersoServerGuiSubkernel::setFirmwareDisplay(
    QPlainTextEdit* firmwareDisplay) {
  FirmwareDisplay = firmwareDisplay;
}

void PersoServerGuiSubkernel::connect() {}

void PersoServerGuiSubkernel::disconnect() {}

void PersoServerGuiSubkernel::echo() {}

void PersoServerGuiSubkernel::logOn() {}

void PersoServerGuiSubkernel::logOut() {}

void PersoServerGuiSubkernel::launchProductionLine() {}

void PersoServerGuiSubkernel::shutdownProductionLine() {}

void PersoServerGuiSubkernel::getProductionLineData() {}

void PersoServerGuiSubkernel::requestBox() {}

void PersoServerGuiSubkernel::getCurrentBoxData() {}

void PersoServerGuiSubkernel::refundCurrentBox() {}

void PersoServerGuiSubkernel::completeCurrentBox() {}

void PersoServerGuiSubkernel::releaseTransponder() {}

void PersoServerGuiSubkernel::rereleaseTransponder() {}

void PersoServerGuiSubkernel::rollbackTransponder() {}

void PersoServerGuiSubkernel::getCurrentTransponderData() {}

void PersoServerGuiSubkernel::getTransponderData() {}

void PersoServerGuiSubkernel::printBoxSticker() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  StringInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param.get());

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
  dialog.getData(*param.get());

  emit clearLogDisplay();
  emit printPalletSticker_signal(param);
}

void PersoServerGuiSubkernel::printLastPalletSticker() {
  emit clearLogDisplay();
  emit printLastPalletSticker_signal();
}

void PersoServerGuiSubkernel::displayProductionLineData(
    const std::shared_ptr<StringDictionary> data) {
  ProductionLine->setData(data);
}

void PersoServerGuiSubkernel::displayBoxData(
    const std::shared_ptr<StringDictionary> data) {
  Box->setData(data);
}

void PersoServerGuiSubkernel::displayTransponderData(
    const std::shared_ptr<StringDictionary> data) {
  Transponder->setData(data);
}

void PersoServerGuiSubkernel::displayFirmware(
    const std::shared_ptr<QByteArray> firmware) {
  if (!FirmwareDisplay) {
    return;
  }

  FirmwareDisplay->setPlainText(firmware->toHex());
}

void PersoServerGuiSubkernel::connectDependecies() {
  PersoServerManager* om = static_cast<PersoServerManager*>(
      GlobalEnvironment::instance()->getObject("PersoServerManager"));

  QObject::connect(this, &PersoServerGuiSubkernel::connect_signal, om,
                   &PersoServerManager::connect);
  QObject::connect(this, &PersoServerGuiSubkernel::disconnect_signal, om,
                   &PersoServerManager::disconnect);

  QObject::connect(this, &PersoServerGuiSubkernel::echo_signal, om,
                   &PersoServerManager::echo);
  QObject::connect(this, &PersoServerGuiSubkernel::logOn_signal, om,
                   &PersoServerManager::logOn);
  QObject::connect(this, &PersoServerGuiSubkernel::logOut_signal, om,
                   &PersoServerManager::logOut);

  QObject::connect(this, &PersoServerGuiSubkernel::launchProductionLine_signal,
                   om, &PersoServerManager::launchProductionLine);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::shutdownProductionLine_signal, om,
                   &PersoServerManager::shutdownProductionLine);
  QObject::connect(this, &PersoServerGuiSubkernel::getProductionLineData_signal,
                   om, &PersoServerManager::getProductionLineData);

  QObject::connect(this, &PersoServerGuiSubkernel::requestBox_signal, om,
                   &PersoServerManager::requestBox);
  QObject::connect(this, &PersoServerGuiSubkernel::getCurrentBoxData_signal, om,
                   &PersoServerManager::getCurrentBoxData);
  QObject::connect(this, &PersoServerGuiSubkernel::refundCurrentBox_signal, om,
                   &PersoServerManager::refundCurrentBox);
  QObject::connect(this, &PersoServerGuiSubkernel::completeCurrentBox_signal,
                   om, &PersoServerManager::completeCurrentBox);

  QObject::connect(this, &PersoServerGuiSubkernel::releaseTransponder_signal,
                   om, &PersoServerManager::releaseTransponder);
  QObject::connect(this, &PersoServerGuiSubkernel::rereleaseTransponder_signal,
                   om, &PersoServerManager::rereleaseTransponder);
  QObject::connect(this, &PersoServerGuiSubkernel::rollbackTransponder_signal,
                   om, &PersoServerManager::rollbackTransponder);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::getCurrentTransponderData_signal,
                   om, &PersoServerManager::getCurrentTransponderData);
  QObject::connect(this, &PersoServerGuiSubkernel::getTransponderData_signal,
                   om, &PersoServerManager::getTransponderData);

  QObject::connect(this, &PersoServerGuiSubkernel::printBoxSticker_signal, om,
                   &PersoServerManager::printBoxSticker);
  QObject::connect(this, &PersoServerGuiSubkernel::printLastBoxSticker_signal,
                   om, &PersoServerManager::printLastBoxSticker);
  QObject::connect(this, &PersoServerGuiSubkernel::printPalletSticker_signal,
                   om, &PersoServerManager::printPalletSticker);
  QObject::connect(this,
                   &PersoServerGuiSubkernel::printLastPalletSticker_signal, om,
                   &PersoServerManager::printLastPalletSticker);
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

  ProductionLine = std::unique_ptr<HashTableModel>(new HashTableModel());
  ProductionLine->setMatchTable(plMatchTable);

  std::shared_ptr<StringDictionary> tMatchTable(new StringDictionary());
  tMatchTable->insert("transponder_sn", "Серийный номер");
  tMatchTable->insert("transponder_ucid", "UCID");
  tMatchTable->insert("transponder_pan", "PAN");
  tMatchTable->insert("box_id", "Идентификатор бокса");
  tMatchTable->insert("transponder_release_counter", "Количество выпусков");
  tMatchTable->insert("issuer_name", "Заказчик");

  Transponder = std::unique_ptr<HashTableModel>(new HashTableModel());
  Transponder->setMatchTable(tMatchTable);

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

  Box = std::unique_ptr<HashTableModel>(new HashTableModel());
  Box->setMatchTable(bMatchTable);
}
