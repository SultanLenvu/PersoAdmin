#include <QDir>
#include <QSettings>

#include "perso_server_connection.h"
#include "production_manager.h"

ProductionManager::ProductionManager(const QString& name)
    : AbstractManager{name} {
  loadSettings();
}

ProductionManager::~ProductionManager() {}

void ProductionManager::onInstanceThreadStarted() {
  createServerConnection();
}

AbstractManager::Type ProductionManager::type() const {
  return Type::Production;
}

void ProductionManager::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  Server->applySettings();
}

void ProductionManager::connectToServer() {
  initOperation("connectToServer");

  ReturnStatus ret;
  ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    processOperationError("connectToServer", ret);
    return;
  }

  completeOperation("connectToServer");
}

void ProductionManager::disconnectFromServer() {
  initOperation("connectToServer");

  Server->disconnect();

  completeOperation("connectToServer");
}

void ProductionManager::launchProductionLine(
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

void ProductionManager::shutdownProductionLine() {
  initOperation("shutdownProductionLine");

  Server->shutdownProductionLine();

  ProductionLineData.clear();
  emit displayBoxData_signal(ProductionLineData);

  completeOperation("shutdownProductionLine");
}

void ProductionManager::getProductionLineData() {
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

void ProductionManager::logOnServer(
    const std::shared_ptr<StringDictionary> param) {
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

void ProductionManager::logOutServer() {
  initOperation("logOutServer");

  if (Server->isConnected()) {
    Server->shutdownProductionLine();
    Server->disconnect();
  }

  completeOperation("logOutServer");
}

void ProductionManager::echoServer() {
  initOperation("echoServer");

  ReturnStatus ret;
  ret = Server->echo();
  if (ret != ReturnStatus::NoError) {
    processOperationError("echoServer", ret);
    return;
  }

  completeOperation("echoServer");
}

void ProductionManager::requestBox() {
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

void ProductionManager::getCurrentBoxData() {
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

void ProductionManager::refundCurrentBox() {
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

void ProductionManager::completeCurrentBox() {
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

void ProductionManager::releaseTransponder() {
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

void ProductionManager::rereleaseTransponder(
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

void ProductionManager::rollbackTransponder() {
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

void ProductionManager::getCurrentTransponderData() {
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

void ProductionManager::getTransponderData(
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

void ProductionManager::printBoxSticker(
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

void ProductionManager::printLastBoxSticker() {
  initOperation("printLastBoxSticker");

  ReturnStatus ret;
  ret = Server->printLastBoxSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastBoxSticker", ret);
    return;
  }

  completeOperation("printLastBoxSticker");
}

void ProductionManager::printPalletSticker(
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

void ProductionManager::printLastPalletSticker() {
  initOperation("printLastPalletSticker");

  ReturnStatus ret;
  ret = Server->printLastPalletSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastPalletSticker", ret);
    return;
  }

  completeOperation("printLastPalletSticker");
}

void ProductionManager::onServerDisconnected() {
  ProductionLineData.clear();
  emit displayProductionLineData_signal(ProductionLineData);

  BoxData.clear();
  emit displayBoxData_signal(BoxData);

  TransponderData.clear();
  emit displayTransponderData_signal(TransponderData);
}

void ProductionManager::loadSettings() {}

ReturnStatus ProductionManager::checkConfig() {
  sendLog("Проверка конфигурации.");

  ReturnStatus ret = ReturnStatus::NoError;

  sendLog("Проверка конфигурации успешно завершена.");
  return ret;
}

void ProductionManager::createServerConnection() {
  Server = std::unique_ptr<AbstractServerConnection>(
      new PersoServerConnection("PersoServerConnection"));

  connect(Server.get(), &AbstractServerConnection::disconnected, this,
          &ProductionManager::onServerDisconnected);
}

void AbstractManager::connectDependencies() {}
