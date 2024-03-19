#include "assembly_unit_gui_subkernel.h"
#include "authorization_dialog.h"
#include "global_environment.h"

AssemblyUnitGuiSubkernel::AssemblyUnitGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  createModels();
  connectDependecies();
}

HashTableModel& AssemblyUnitGuiSubkernel::productionLine() {
  return ProductionLine;
}

HashTableModel& AssemblyUnitGuiSubkernel::box() {
  return Box;
}

HashTableModel& AssemblyUnitGuiSubkernel::transponder() {
  return Transponder;
}

void AssemblyUnitGuiSubkernel::logOn() {
  std::shared_ptr<StringDictionary> param(new StringDictionary());

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(*param);

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
    const std::shared_ptr<StringDictionary> data) {}

void AssemblyUnitGuiSubkernel::displayBoxData(
    const std::shared_ptr<StringDictionary> data) {}

void AssemblyUnitGuiSubkernel::displayTransponderData(
    const std::shared_ptr<StringDictionary> data) {}

void AssemblyUnitGuiSubkernel::displayFirmware(
    const std::shared_ptr<QByteArray> firmware) {}

void AssemblyUnitGuiSubkernel::connectDependecies() {
  const ServerConnectionAsyncWrapper* psm =
      static_cast<const ServerConnectionAsyncWrapper*>(
          GlobalEnvironment::instance()->getObject(
              "ServerConnectionAsyncWrapper"));
}

void AssemblyUnitGuiSubkernel::createModels() {}
