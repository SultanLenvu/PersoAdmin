#include "server_connection_async_wrapper.h"

#include <QDir>
#include <QSettings>

#include "perso_server_connection.h"

ServerConnectionAsyncWrapper::ServerConnectionAsyncWrapper(const QString& name)
    : NamedObject{name},
      LoggableObject(name),
      Server(new PersoServerConnection("PersoServerConnection")) {}

ServerConnectionAsyncWrapper::~ServerConnectionAsyncWrapper() {}

void ServerConnectionAsyncWrapper::connect() {
  initOperation("connectToServer");

  ReturnStatus ret;
  ret = Server->connect();
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
    const std::shared_ptr<StringDictionary> param) {
  initOperation("launchProductionLine");

  ReturnStatus ret;
  ret = Server->launchProductionLine(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("launchProductionLine", ret);
    return;
  }

  std::shared_ptr<StringDictionary> pldata(new StringDictionary());
  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    emit boxDataReady(pldata);
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(pldata);

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
  std::shared_ptr<StringDictionary> pldata(new StringDictionary());

  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getProductionLineData", ret);
    return;
  }

  emit productionLineDataReady(pldata);

  completeOperation("getProductionLineData");
}

void ServerConnectionAsyncWrapper::logOn(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("logOnServer");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> pldata(new StringDictionary());

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

  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(pldata);
  emit authorizationCompleted();

  completeOperation("logOnServer");
}

void ServerConnectionAsyncWrapper::logOut() {
  initOperation("logOutServer");

  if (Server->isConnected()) {
    Server->shutdownProductionLine();
    Server->disconnect();
  }

  completeOperation("logOutServer");
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

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> bdata(new StringDictionary());
  std::shared_ptr<StringDictionary> pldata(new StringDictionary());
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

  ret = Server->requestBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  ret = Server->getCurrentBoxData(*bdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  emit boxDataReady(bdata);

  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  emit productionLineDataReady(pldata);

  // Если в боксе есть собранные транспондеры
  if (bdata->value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(*tdata);
    if (ret != ReturnStatus::NoError) {
      processOperationError("requestBox", ret);
      return;
    }

    emit transponderDataReady(tdata);
  }

  completeOperation("requestBox");
}

void ServerConnectionAsyncWrapper::getCurrentBoxData() {
  initOperation("getCurrentBoxData");

  std::shared_ptr<StringDictionary> bdata(new StringDictionary());

  ReturnStatus ret = Server->getCurrentBoxData(*bdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getCurrentBoxData", ret);
    return;
  }

  emit boxDataReady(bdata);

  completeOperation("getCurrentBoxData");
}

void ServerConnectionAsyncWrapper::refundCurrentBox() {
  initOperation("refundCurrentBox");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> pldata(new StringDictionary());

  ret = Server->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("refundCurrentBox", ret);
    return;
  }

  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(pldata);

  completeOperation("refundCurrentBox");
}

void ServerConnectionAsyncWrapper::completeCurrentBox() {
  initOperation("completeCurrentBox");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> pldata(new StringDictionary());

  ret = Server->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("completeCurrentBox", ret);
    return;
  }

  ret = Server->getProductionLineData(*pldata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(pldata);

  completeOperation("completeCurrentBox");
}

void ServerConnectionAsyncWrapper::releaseTransponder() {
  initOperation("releaseTransponder");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> bdata(new StringDictionary());
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

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
  ret = Server->getCurrentBoxData(*bdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getCurrentBoxData", ret);
    return;
  }

  emit boxDataReady(bdata);

  // Запрашиваем данные выпущенного транспондера
  ret = Server->getCurrentTransponderData(*tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Данные выпускаемого транспондера получены."));

  emit transponderDataReady(tdata);

  // Печатаем стикер
  ret = ReturnStatus::StickerPrinterConnectionError;
  emit printTransponderSticker_signal(*tdata, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Стикер распечатан."));

  completeOperation("releaseTransponder");
}

void ServerConnectionAsyncWrapper::rereleaseTransponder(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("rereleaseTransponder");
  sendLog("Выпуск транспондера. ");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

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
  ret = Server->getTransponderData(requestParam, *tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }

  // Запрашиваем отображение данных транспондера
  emit transponderDataReady(tdata);

  // Печатаем стикер
  emit printTransponderSticker_signal(*tdata, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  completeOperation("rereleaseTransponder");
}

void ServerConnectionAsyncWrapper::rollbackTransponder() {
  initOperation("rollbackTransponder");
  sendLog("Откат производственной линии. ");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> bdata(new StringDictionary());
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

  ret = Server->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(*bdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  emit boxDataReady(bdata);

  // Если в боксе есть собранные транспондеры
  if (bdata->value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(*tdata);
    if (ret != ReturnStatus::NoError) {
      processOperationError("rollbackTransponder", ret);
      return;
    }

    emit transponderDataReady(tdata);
  }

  completeOperation("rollbackTransponder");
}

void ServerConnectionAsyncWrapper::getCurrentTransponderData() {
  initOperation("getCurrentTransponderData");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

  ret = Server->getCurrentTransponderData(*tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getCurrentTransponderData", ret);
    return;
  }

  emit transponderDataReady(tdata);

  completeOperation("getCurrentTransponderData");
}

void ServerConnectionAsyncWrapper::getTransponderData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getTransponderData");

  ReturnStatus ret = ReturnStatus::NoError;
  std::shared_ptr<StringDictionary> tdata(new StringDictionary());

  ret = Server->getTransponderData(*param, *tdata);
  if (ret != ReturnStatus::NoError) {
    processOperationError("getTransponderData", ret);
    return;
  }

  emit transponderDataReady(tdata);

  completeOperation("getTransponderData");
}

void ServerConnectionAsyncWrapper::printBoxSticker(
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

void ServerConnectionAsyncWrapper::onServerDisconnected() {}

ReturnStatus ServerConnectionAsyncWrapper::checkConfig() {
  sendLog("Проверка конфигурации.");

  ReturnStatus ret = ReturnStatus::NoError;

  sendLog("Проверка конфигурации успешно завершена.");
  return ret;
}
