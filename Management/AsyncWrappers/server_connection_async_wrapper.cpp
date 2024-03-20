#include "server_connection_async_wrapper.h"
#include "assembly_unit_gui_subkernel.h"
#include "global_environment.h"
#include "perso_server_connection.h"

#include <QDir>
#include <QSettings>

ServerConnectionAsyncWrapper::ServerConnectionAsyncWrapper(const QString& name)
    : ProgressableAsyncWrapper{name},
      Server(new PersoServerConnection("PersoServerConnection")) {
  // Шерим подключение к серверу
  GlobalEnvironment::instance()->registerSharedObject<PersoServerConnection>(
      std::static_pointer_cast<PersoServerConnection>(Server));
}

void ServerConnectionAsyncWrapper::connect() {
  initOperation("connectToServer");

  ReturnStatus ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    processOperationError("connectToServer", ret);
    return;
  }

  completeOperation("connectToServer");
}

void ServerConnectionAsyncWrapper::disconnect() {
  initOperation("connectToServer");

  Server->disconnect();

  completeOperation("connectToServer");
}

void ServerConnectionAsyncWrapper::launchProductionLine(
    const StringDictionary& param) {
  initOperation("launchProductionLine");

  ReturnStatus ret = Server->launchProductionLine(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("launchProductionLine", ret);
    return;
  }

  completeOperation("launchProductionLine");
}

void ServerConnectionAsyncWrapper::shutdownProductionLine() {
  initOperation("shutdownProductionLine");

  Server->shutdownProductionLine();

  completeOperation("shutdownProductionLine");
}

void ServerConnectionAsyncWrapper::getProductionLineData() {
  initOperation("getProductionLineData");

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary pldata;

  ret = Server->getProductionLineData(pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getProductionLineData", ret);
    return;
  }

  emit productionLineDataReady(pldata);

  completeOperation("getProductionLineData");
}

void ServerConnectionAsyncWrapper::echo() {
  initOperation("echoServer");

  ReturnStatus ret;
  ret = Server->echo();
  if (ret != ReturnStatus::NoError) {
    processOperationError("echoServer", ret);
    return;
  }

  completeOperation("echoServer");
}

void ServerConnectionAsyncWrapper::requestBox() {
  initOperation("requestBox");

  ReturnStatus ret = Server->requestBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  completeOperation("requestBox");
}

void ServerConnectionAsyncWrapper::getCurrentBoxData() {
  initOperation("getCurrentBoxData");

  StringDictionary bdata;
  ReturnStatus ret = Server->getCurrentBoxData(bdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getCurrentBoxData", ret);
    return;
  }

  emit boxDataReady(bdata);

  completeOperation("getCurrentBoxData");
}

void ServerConnectionAsyncWrapper::refundCurrentBox() {
  initOperation("refundCurrentBox");

  ReturnStatus ret = Server->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("refundCurrentBox", ret);
    return;
  }

  completeOperation("refundCurrentBox");
}

void ServerConnectionAsyncWrapper::completeCurrentBox() {
  initOperation("completeCurrentBox");

  ReturnStatus ret = Server->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("completeCurrentBox", ret);
    return;
  }

  completeOperation("completeCurrentBox");
}

void ServerConnectionAsyncWrapper::releaseTransponder() {
  initOperation("releaseTransponder");

  StringDictionary result;
  ReturnStatus ret = Server->releaseTransponder(result);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }

  completeOperation("releaseTransponder");
}

void ServerConnectionAsyncWrapper::confirmTransponderRelease(
    const StringDictionary& param) {
  initOperation("confirmTransponderRelease");

  ReturnStatus ret = Server->confirmTransponderRelease(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("confirmTransponderRelease", ret);
    return;
  }

  completeOperation("confirmTransponderRelease");
}

void ServerConnectionAsyncWrapper::rereleaseTransponder(
    const StringDictionary& param) {
  initOperation("rereleaseTransponder");

  StringDictionary result;
  ReturnStatus ret = Server->rereleaseTransponder(param, result);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  completeOperation("rereleaseTransponder");
}

void ServerConnectionAsyncWrapper::rollbackTransponder() {
  initOperation("rollbackTransponder");

  ReturnStatus ret = Server->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  completeOperation("rollbackTransponder");
}

void ServerConnectionAsyncWrapper::getCurrentTransponderData() {
  initOperation("getCurrentTransponderData");

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary tdata;

  ret = Server->getCurrentTransponderData(tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getCurrentTransponderData", ret);
    return;
  }

  emit transponderDataReady(tdata);

  completeOperation("getCurrentTransponderData");
}

void ServerConnectionAsyncWrapper::getTransponderData(
    const StringDictionary& param) {
  initOperation("getTransponderData");

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary tdata;

  ret = Server->getTransponderData(param, tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getTransponderData", ret);
    return;
  }

  emit transponderDataReady(tdata);

  completeOperation("getTransponderData");
}

void ServerConnectionAsyncWrapper::printBoxSticker(
    const StringDictionary& param) {
  initOperation("printBoxSticker");

  ReturnStatus ret;
  ret = Server->printBoxSticker(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printBoxSticker");
}

void ServerConnectionAsyncWrapper::printLastBoxSticker() {
  initOperation("printLastBoxSticker");

  ReturnStatus ret;
  ret = Server->printLastBoxSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastBoxSticker", ret);
    return;
  }

  completeOperation("printLastBoxSticker");
}

void ServerConnectionAsyncWrapper::printPalletSticker(
    const StringDictionary& param) {
  initOperation("printPalletSticker");

  ReturnStatus ret;
  ret = Server->printPalletSticker(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printPalletSticker", ret);
    return;
  }

  completeOperation("printPalletSticker");
}

void ServerConnectionAsyncWrapper::printLastPalletSticker() {
  initOperation("printLastPalletSticker");

  ReturnStatus ret;
  ret = Server->printLastPalletSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastPalletSticker", ret);
    return;
  }

  completeOperation("printLastPalletSticker");
}

ReturnStatus ServerConnectionAsyncWrapper::checkConfig() {
  sendLog("Проверка конфигурации.");

  ReturnStatus ret = ReturnStatus::NoError;

  sendLog("Проверка конфигурации успешно завершена.");
  return ret;
}

void ServerConnectionAsyncWrapper::connectDependecies() {
  AssemblyUnitGuiSubkernel* augs =
      GlobalEnvironment::instance()->getObject<AssemblyUnitGuiSubkernel>(
          "AssemblyUnitGuiSubkernel");

  QObject::connect(this, &ServerConnectionAsyncWrapper::productionLineDataReady,
                   augs, &AssemblyUnitGuiSubkernel::displayProductionLineData);
  QObject::connect(this, &ServerConnectionAsyncWrapper::boxDataReady, augs,
                   &AssemblyUnitGuiSubkernel::displayBoxData);
  QObject::connect(this, &ServerConnectionAsyncWrapper::transponderDataReady,
                   augs, &AssemblyUnitGuiSubkernel::displayTransponderData);
  QObject::connect(this, &ServerConnectionAsyncWrapper::firwareReady, augs,
                   &AssemblyUnitGuiSubkernel::displayFirmware);
}
