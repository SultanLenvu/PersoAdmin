#include "perso_server_manager.h"

#include <QDir>
#include <QSettings>

#include "perso_server_connection.h"

PersoServerManager::PersoServerManager(const QString& name)
    : AbstractManager{name} {
  loadSettings();
}

PersoServerManager::~PersoServerManager() {}

void PersoServerManager::onInstanceThreadStarted() {
  createServerConnection();
}

void PersoServerManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Server->applySettings();
}

void PersoServerManager::connect() {
  initOperation("connectToServer");

  ReturnStatus ret;
  ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    processOperationError("connectToServer", ret);
    return;
  }

  completeOperation("connectToServer");
}

void PersoServerManager::disconnect() {
  initOperation("connectToServer");

  Server->disconnect();

  completeOperation("connectToServer");
}

void PersoServerManager::launchProductionLine(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("launchProductionLine");

  ReturnStatus ret;
  ret = Server->launchProductionLine(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("launchProductionLine", ret);
    return;
  }

  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayBoxData_signal(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  completeOperation("launchProductionLine");
}

void PersoServerManager::shutdownProductionLine() {
  initOperation("shutdownProductionLine");

  Server->shutdownProductionLine();

  ProductionLineData.clear();
  emit displayBoxData_signal(ProductionLineData);

  completeOperation("shutdownProductionLine");
}

void PersoServerManager::getProductionLineData() {
  initOperation("getProductionLineData");

  ReturnStatus ret;
  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayProductionLineData_signal(ProductionLineData);
    processOperationError("getProductionLineData", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  completeOperation("getProductionLineData");
}

void PersoServerManager::logOn(const std::shared_ptr<StringDictionary> param) {
  initOperation("logOnServer");

  ReturnStatus ret;
  ret = checkConfig();
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  ret = Server->launchProductionLine(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayBoxData_signal(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  emit authorizationCompleted();
  completeOperation("logOnServer");
}

void PersoServerManager::logOut() {
  initOperation("logOutServer");

  if (Server->isConnected()) {
    Server->shutdownProductionLine();
    Server->disconnect();
  }

  completeOperation("logOutServer");
}

void PersoServerManager::echo() {
  initOperation("echoServer");

  ReturnStatus ret;
  ret = Server->echo();
  if (ret != ReturnStatus::NoError) {
    processOperationError("echoServer", ret);
    return;
  }

  completeOperation("echoServer");
}

void PersoServerManager::requestBox() {
  initOperation("requestBox");

  ReturnStatus ret;

  ret = Server->requestBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  ret = Server->getCurrentBoxData(BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData.clear();
    emit displayBoxData_signal(BoxData);
    processOperationError("requestBox", ret);
    return;
  }

  emit displayBoxData_signal(BoxData);

  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayBoxData_signal(ProductionLineData);
    processOperationError("requestBox", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  // Если в боксе есть собранные транспондеры
  if (BoxData.value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(TransponderData);
    if (ret != ReturnStatus::NoError) {
      TransponderData.clear();
      emit displayTransponderData_signal(TransponderData);
      processOperationError("requestBox", ret);
      return;
    }

    emit displayTransponderData_signal(TransponderData);
  }

  completeOperation("requestBox");
}

void PersoServerManager::getCurrentBoxData() {
  initOperation("getCurrentBoxData");

  ReturnStatus ret = Server->getCurrentBoxData(BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData.clear();
    emit displayBoxData_signal(BoxData);
    processOperationError("getCurrentBoxData", ret);
    sendLog("Не удалось получить данные текущего бокса. ");
    return;
  }

  emit displayBoxData_signal(BoxData);

  completeOperation("getCurrentBoxData");
}

void PersoServerManager::refundCurrentBox() {
  initOperation("refundCurrentBox");

  ReturnStatus ret;
  ret = Server->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("refundCurrentBox", ret);
    return;
  }

  BoxData.clear();
  emit displayBoxData_signal(BoxData);

  TransponderData.clear();
  emit displayTransponderData_signal(TransponderData);

  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayBoxData_signal(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  completeOperation("refundCurrentBox");
}

void PersoServerManager::completeCurrentBox() {
  initOperation("completeCurrentBox");

  ReturnStatus ret;
  ret = Server->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("completeCurrentBox", ret);
    return;
  }

  BoxData.clear();
  emit displayBoxData_signal(BoxData);

  TransponderData.clear();
  emit displayTransponderData_signal(TransponderData);

  ret = Server->getProductionLineData(ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData.clear();
    emit displayBoxData_signal(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit displayProductionLineData_signal(ProductionLineData);

  completeOperation("completeCurrentBox");
}

void PersoServerManager::releaseTransponder() {
  initOperation("releaseTransponder");

  ReturnStatus ret;
  QString ucid;
  StringDictionary result;
  StringDictionary param;

  // Выпускаем транспондер
  ret = Server->releaseTransponder(result);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Транспондер выпущен."));

  // Подтверждаем выпуск транспондера
  param.insert("transponder_ucid", ucid);
  ret = Server->confirmTransponderRelease(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Выпуск транспондера подтвержден."));

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData.clear();
    emit displayBoxData_signal(BoxData);
    processOperationError("getCurrentBoxData", ret);
    return;
  }

  emit displayBoxData_signal(BoxData);

  // Запрашиваем данные выпущенного транспондера
  ret = Server->getCurrentTransponderData(TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData.clear();
    emit displayTransponderData_signal(TransponderData);
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Данные выпускаемого транспондера получены."));

  emit displayTransponderData_signal(TransponderData);

  // Печатаем стикер
  ret = ReturnStatus::StickerPrinterConnectionError;
  emit printTransponderSticker_signal(TransponderData, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Стикер распечатан."));

  completeOperation("releaseTransponder");
}

void PersoServerManager::rereleaseTransponder(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("rereleaseTransponder");
  sendLog("Выпуск транспондера. ");

  ReturnStatus ret;
  QString ucid;
  StringDictionary result;
  StringDictionary requestParam;

  // Перевыпускаем транспондер
  requestParam.insert("transponder_pan", param->value("transponder_pan"));
  ret = Server->rereleaseTransponder(requestParam, result);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  // Подтверждаем перевыпуск транспондера
  requestParam.insert("transponder_ucid", ucid);
  ret = Server->confirmTransponderRerelease(requestParam);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  // Запрашиваем данные перевыпущенного транспондера
  requestParam.remove("transponder_ucid");
  ret = Server->getTransponderData(requestParam, TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData.clear();
    emit displayTransponderData_signal(TransponderData);
    processOperationError("releaseTransponder", ret);
    return;
  }

  // Запрашиваем отображение данных транспондера
  emit displayTransponderData_signal(TransponderData);

  // Печатаем стикер
  emit printTransponderSticker_signal(TransponderData, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  completeOperation("rereleaseTransponder");
}

void PersoServerManager::rollbackTransponder() {
  initOperation("rollbackTransponder");
  sendLog("Откат производственной линии. ");

  ReturnStatus ret;
  ret = Server->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData.clear();
    emit displayBoxData_signal(BoxData);
    processOperationError("rollbackTransponder", ret);
    return;
  }

  emit displayBoxData_signal(BoxData);

  // Если в боксе есть собранные транспондеры
  if (BoxData.value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(TransponderData);
    if (ret != ReturnStatus::NoError) {
      TransponderData.clear();
      emit displayTransponderData_signal(TransponderData);
      processOperationError("rollbackTransponder", ret);
      return;
    }

    emit displayTransponderData_signal(TransponderData);
  } else {
    TransponderData.clear();
    emit displayTransponderData_signal(TransponderData);
  }

  completeOperation("rollbackTransponder");
}

void PersoServerManager::getCurrentTransponderData() {
  initOperation("getCurrentTransponderData");

  ReturnStatus ret;
  ret = Server->getCurrentTransponderData(TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData.clear();
    emit displayTransponderData_signal(TransponderData);
    processOperationError("getCurrentTransponderData", ret);
    return;
  }

  emit displayTransponderData_signal(TransponderData);

  completeOperation("getCurrentTransponderData");
}

void PersoServerManager::getTransponderData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getTransponderData");

  ReturnStatus ret;
  ret = Server->getTransponderData(*param, TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData.clear();
    emit displayTransponderData_signal(TransponderData);
    processOperationError("getTransponderData", ret);
    return;
  }

  emit displayTransponderData_signal(TransponderData);

  completeOperation("getTransponderData");
}

void PersoServerManager::printBoxSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printBoxSticker");

  ReturnStatus ret;
  ret = Server->printBoxSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printBoxSticker", ret);
    return;
  }

  completeOperation("printBoxSticker");
}

void PersoServerManager::printLastBoxSticker() {
  initOperation("printLastBoxSticker");

  ReturnStatus ret;
  ret = Server->printLastBoxSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastBoxSticker", ret);
    return;
  }

  completeOperation("printLastBoxSticker");
}

void PersoServerManager::printPalletSticker(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("printPalletSticker");

  ReturnStatus ret;
  ret = Server->printPalletSticker(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("printPalletSticker", ret);
    return;
  }

  completeOperation("printPalletSticker");
}

void PersoServerManager::printLastPalletSticker() {
  initOperation("printLastPalletSticker");

  ReturnStatus ret;
  ret = Server->printLastPalletSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastPalletSticker", ret);
    return;
  }

  completeOperation("printLastPalletSticker");
}

void PersoServerManager::onServerDisconnected() {
  ProductionLineData.clear();
  emit displayProductionLineData_signal(ProductionLineData);

  BoxData.clear();
  emit displayBoxData_signal(BoxData);

  TransponderData.clear();
  emit displayTransponderData_signal(TransponderData);
}

void PersoServerManager::loadSettings() {}

ReturnStatus PersoServerManager::checkConfig() {
  sendLog("Проверка конфигурации.");

  ReturnStatus ret = ReturnStatus::NoError;

  sendLog("Проверка конфигурации успешно завершена.");
  return ret;
}

void PersoServerManager::createServerConnection() {
  Server = std::unique_ptr<AbstractServerConnection>(
      new PersoServerConnection("PersoServerConnection"));

  QObject::connect(Server.get(), &AbstractServerConnection::disconnected, this,
                   &PersoServerManager::onServerDisconnected);
}
