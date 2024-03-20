#include "server_connection_gui_subkernel.h"
#include "authorization_dialog.h"
#include "confirm_transponder_rerelease_input_dialog.h"
#include "global_environment.h"
#include "numeric_string_checker.h"
#include "server_connection_async_wrapper.h"
#include "string_input_dialog.h"

ServerConnectionGuiSubkernel::ServerConnectionGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
  createCommandMethod();
}

void ServerConnectionGuiSubkernel::executeCommand(const QString& name) {
  emit clearLogDisplay();

  (this->*CommandMethods.value(name))();
}

void ServerConnectionGuiSubkernel::onServerDisconnected() {
  QMessageBox::critical(nullptr, "Ошибка", "Соединение с сервером оборвалось.",
                        QMessageBox::Ok);
}

void ServerConnectionGuiSubkernel::connect() {
  emit clearLogDisplay();
  emit connect_signal();
}

void ServerConnectionGuiSubkernel::disconnect() {
  emit clearLogDisplay();
  emit disconnect_signal();
}

void ServerConnectionGuiSubkernel::echo() {
  emit clearLogDisplay();
  emit echo_signal();
}

void ServerConnectionGuiSubkernel::launchProductionLine() {
  StringDictionary param;

  AuthorizationDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit launchProductionLine_signal(param);
}

void ServerConnectionGuiSubkernel::shutdownProductionLine() {
  emit clearLogDisplay();
  emit shutdownProductionLine_signal();
}

void ServerConnectionGuiSubkernel::getProductionLineData() {
  emit clearLogDisplay();
  emit getProductionLineData_signal();
}

void ServerConnectionGuiSubkernel::requestBox() {
  emit clearLogDisplay();
  emit requestBox_signal();
}

void ServerConnectionGuiSubkernel::getCurrentBoxData() {
  emit clearLogDisplay();
  emit getCurrentBoxData_signal();
}

void ServerConnectionGuiSubkernel::refundCurrentBox() {
  emit clearLogDisplay();
  emit refundCurrentBox_signal();
}

void ServerConnectionGuiSubkernel::completeCurrentBox() {
  emit clearLogDisplay();
  emit completeCurrentBox_signal();
}

void ServerConnectionGuiSubkernel::releaseTransponder() {
  emit clearLogDisplay();
  emit releaseTransponder_signal();
}

void ServerConnectionGuiSubkernel::confirmTransponderRelease() {
  StringDictionary param;

  UcidChecker checker;
  StringInputDialog dialog("transponder_ucid", &checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit confirmTransponderRelease_signal(param);
  emit clearLogDisplay();
}

void ServerConnectionGuiSubkernel::rereleaseTransponder() {
  StringDictionary param;

  NumericStringChecker checker;
  StringInputDialog dialog("transponder_pan", &checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit rereleaseTransponder_signal(param);
}

void ServerConnectionGuiSubkernel::confirmTransponderRerelease() {
  StringDictionary param;

  ConfirmTransponderRereleaseInputDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit confirmTransponderRerelease_signal(param);
}

void ServerConnectionGuiSubkernel::rollbackTransponder() {
  emit clearLogDisplay();
  emit rollbackTransponder_signal();
}

void ServerConnectionGuiSubkernel::getCurrentTransponderData() {
  emit clearLogDisplay();
  emit getCurrentTransponderData_signal();
}

void ServerConnectionGuiSubkernel::getTransponderData() {
  StringDictionary param;

  StringInputDialog dialog("transponder_pan");
  NumericStringChecker checker;
  dialog.setChecker(&checker);
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit getTransponderData_signal(param);
}

void ServerConnectionGuiSubkernel::printBoxSticker() {
  StringDictionary param;

  StringInputDialog dialog("transponder_pan");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit printBoxSticker_signal(param);
}

void ServerConnectionGuiSubkernel::printLastBoxSticker() {
  emit clearLogDisplay();
  emit printLastBoxSticker_signal();
}

void ServerConnectionGuiSubkernel::printPalletSticker() {
  StringDictionary param;

  StringInputDialog dialog("transponder_pan");
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
  dialog.getData(param);

  emit clearLogDisplay();
  emit printPalletSticker_signal(param);
}

void ServerConnectionGuiSubkernel::printLastPalletSticker() {
  emit clearLogDisplay();
  emit printLastPalletSticker_signal();
}

void ServerConnectionGuiSubkernel::connectDependecies() {
  const ServerConnectionAsyncWrapper* psm =
      GlobalEnvironment::instance()
          ->getObject<const ServerConnectionAsyncWrapper>(
              "ServerConnectionAsyncWrapper");

  // К менеджерам
  QObject::connect(this, &ServerConnectionGuiSubkernel::connect_signal, psm,
                   &ServerConnectionAsyncWrapper::connect);
  QObject::connect(this, &ServerConnectionGuiSubkernel::disconnect_signal, psm,
                   &ServerConnectionAsyncWrapper::disconnect);

  QObject::connect(this, &ServerConnectionGuiSubkernel::echo_signal, psm,
                   &ServerConnectionAsyncWrapper::echo);

  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::launchProductionLine_signal,
                   psm, &ServerConnectionAsyncWrapper::launchProductionLine);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::shutdownProductionLine_signal,
                   psm, &ServerConnectionAsyncWrapper::shutdownProductionLine);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::getProductionLineData_signal,
                   psm, &ServerConnectionAsyncWrapper::getProductionLineData);

  QObject::connect(this, &ServerConnectionGuiSubkernel::requestBox_signal, psm,
                   &ServerConnectionAsyncWrapper::requestBox);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::getCurrentBoxData_signal, psm,
                   &ServerConnectionAsyncWrapper::getCurrentBoxData);
  QObject::connect(this, &ServerConnectionGuiSubkernel::refundCurrentBox_signal,
                   psm, &ServerConnectionAsyncWrapper::refundCurrentBox);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::completeCurrentBox_signal,
                   psm, &ServerConnectionAsyncWrapper::completeCurrentBox);

  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::releaseTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::releaseTransponder);
  QObject::connect(
      this, &ServerConnectionGuiSubkernel::confirmTransponderRelease_signal,
      psm, &ServerConnectionAsyncWrapper::confirmTransponderRelease);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::rereleaseTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::rereleaseTransponder);
  QObject::connect(
      this, &ServerConnectionGuiSubkernel::confirmTransponderRerelease_signal,
      psm, &ServerConnectionAsyncWrapper::rereleaseTransponder);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::rollbackTransponder_signal,
                   psm, &ServerConnectionAsyncWrapper::rollbackTransponder);
  QObject::connect(
      this, &ServerConnectionGuiSubkernel::getCurrentTransponderData_signal,
      psm, &ServerConnectionAsyncWrapper::getCurrentTransponderData);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::getTransponderData_signal,
                   psm, &ServerConnectionAsyncWrapper::getTransponderData);

  QObject::connect(this, &ServerConnectionGuiSubkernel::printBoxSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printBoxSticker);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::printLastBoxSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printLastBoxSticker);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::printPalletSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printPalletSticker);
  QObject::connect(this,
                   &ServerConnectionGuiSubkernel::printLastPalletSticker_signal,
                   psm, &ServerConnectionAsyncWrapper::printLastPalletSticker);
}

void ServerConnectionGuiSubkernel::createCommandMethod() {
  CommandMethods["connect"] = &ServerConnectionGuiSubkernel::connect;
  CommandMethods["disconnect"] = &ServerConnectionGuiSubkernel::disconnect;

  CommandMethods["echo"] = &ServerConnectionGuiSubkernel::echo;
  CommandMethods["launch_production_line"] =
      &ServerConnectionGuiSubkernel::launchProductionLine;
  CommandMethods["shutdown_production_line"] =
      &ServerConnectionGuiSubkernel::shutdownProductionLine;
  CommandMethods["get_production_line_data"] =
      &ServerConnectionGuiSubkernel::getProductionLineData;

  CommandMethods["request_box"] = &ServerConnectionGuiSubkernel::requestBox;
  CommandMethods["get_current_box_data"] =
      &ServerConnectionGuiSubkernel::getCurrentBoxData;
  CommandMethods["refund_current_box"] =
      &ServerConnectionGuiSubkernel::refundCurrentBox;
  CommandMethods["complete_current_box"] =
      &ServerConnectionGuiSubkernel::completeCurrentBox;

  CommandMethods["release_transponder"] =
      &ServerConnectionGuiSubkernel::releaseTransponder;
  CommandMethods["confirm_transponder_release"] =
      &ServerConnectionGuiSubkernel::confirmTransponderRelease;
  CommandMethods["rerelease_transponder"] =
      &ServerConnectionGuiSubkernel::rereleaseTransponder;
  CommandMethods["confirm_transponder_rerelease"] =
      &ServerConnectionGuiSubkernel::confirmTransponderRerelease;
  CommandMethods["rollback_transponder"] =
      &ServerConnectionGuiSubkernel::rollbackTransponder;
  CommandMethods["get_current_transponder_data"] =
      &ServerConnectionGuiSubkernel::getCurrentTransponderData;
  CommandMethods["get_transponder_data"] =
      &ServerConnectionGuiSubkernel::getTransponderData;

  CommandMethods["print_box_sticker"] =
      &ServerConnectionGuiSubkernel::printBoxSticker;
  CommandMethods["print_last_box_sticker"] =
      &ServerConnectionGuiSubkernel::printLastBoxSticker;
  CommandMethods["print_pallet_sticker"] =
      &ServerConnectionGuiSubkernel::printPalletSticker;
  CommandMethods["print_last_pallet_sticker"] =
      &ServerConnectionGuiSubkernel::printLastPalletSticker;
}
