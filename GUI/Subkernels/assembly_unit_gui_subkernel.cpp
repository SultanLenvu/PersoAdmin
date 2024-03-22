#include "assembly_unit_gui_subkernel.h"
#include "assembly_unit_manager_async_wrapper.h"
#include "authorization_dialog.h"
#include "global_environment.h"

AssemblyUnitGuiSubkernel::AssemblyUnitGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  createModels();
  connectDependecies();
}

void AssemblyUnitGuiSubkernel::logOn() {
  StringDictionary param;

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit logOn_signal(param);
}

void AssemblyUnitGuiSubkernel::logOut() {
  emit clearLogDisplay();
  emit logOut_signal();
}

void AssemblyUnitGuiSubkernel::requestBox() {}

void AssemblyUnitGuiSubkernel::refundCurrentBox() {}

void AssemblyUnitGuiSubkernel::completeCurrentBox() {}

void AssemblyUnitGuiSubkernel::releaseTransponder() {}

void AssemblyUnitGuiSubkernel::rereleaseTransponder() {}

void AssemblyUnitGuiSubkernel::rollbackTransponder() {}

void AssemblyUnitGuiSubkernel::displayProductionLineData(
    const StringDictionary& data) {
  ProductionLineModel.setData(data);
}

void AssemblyUnitGuiSubkernel::displayBoxData(const StringDictionary& data) {
  BoxModel.setData(data);
}

void AssemblyUnitGuiSubkernel::displayTransponderData(
    const StringDictionary& data) {
  TransponderModel.setData(data);
}

void AssemblyUnitGuiSubkernel::displayFirmware(const QStringList& firmware) {
  FirmwareModel.setStringList(firmware);
}

void AssemblyUnitGuiSubkernel::createModels() {
  StringDictionary plMatchTable;
  plMatchTable.insert("production_line_id", "Идентификатор");
  plMatchTable.insert("production_line_login", "Логин");
  plMatchTable.insert("production_line_in_process", "В процессе сборки");
  plMatchTable.insert("production_line_ns", "Сотрудник");
  plMatchTable.insert("today_assembled_boxes", "Собрано боксов за сегодня");
  plMatchTable.insert("transponder_id", "Идентификатор транспондера");
  plMatchTable.insert("box_id", "Идентификатор бокса");

  ProductionLineModel.setMatchTable(plMatchTable);

  StringDictionary tMatchTable;
  tMatchTable.insert("transponder_sn", "Серийный номер");
  tMatchTable.insert("transponder_ucid", "UCID");
  tMatchTable.insert("transponder_pan", "PAN");
  tMatchTable.insert("box_id", "Идентификатор бокса");
  tMatchTable.insert("transponder_release_counter", "Количество выпусков");
  tMatchTable.insert("issuer_name", "Заказчик");

  TransponderModel.setMatchTable(tMatchTable);

  StringDictionary bMatchTable;
  bMatchTable.insert("box_id", "ID бокса");
  bMatchTable.insert("box_in_process", "В процессе");
  bMatchTable.insert("box_quantity", "Емкость бокса");

  bMatchTable.insert("box_assembled_units", "Собрано транспондеров");
  bMatchTable.insert("box_assembling_start", "Начало сборки");
  bMatchTable.insert("box_assembling_end", "Конец сборки");

  bMatchTable.insert("first_transponder_sn", "Первый транспондер");
  bMatchTable.insert("last_transponder_sn", "Последний транспондер");

  bMatchTable.insert("pallet_id", "ID паллеты");
  bMatchTable.insert("production_line_id", "ID производственной линии");

  BoxModel.setMatchTable(bMatchTable);
}

void AssemblyUnitGuiSubkernel::connectDependecies() {
  AssemblyUnitManagerAsyncWrapper* psm =
      GlobalEnvironment::instance()->getObject<AssemblyUnitManagerAsyncWrapper>(
          "AssemblyUnitManagerAsyncWrapper");

  QObject::connect(this, &AssemblyUnitGuiSubkernel::logOn_signal, psm,
                   &AssemblyUnitManagerAsyncWrapper::logOn);
  QObject::connect(this, &AssemblyUnitGuiSubkernel::logOut_signal, psm,
                   &AssemblyUnitManagerAsyncWrapper::logOut);
}
