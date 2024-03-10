#include "perso_server_async_wrapper.h"

#include <QDir>
#include <QSettings>

#include "perso_server_connection.h"

PersoServerAsyncWrapper::PersoServerAsyncWrapper(const QString& name)
    : NamedObject{name},
      LoggableObject(name),
      ProductionLineData(new StringDictionary()),
      BoxData(new StringDictionary()),
      TransponderData(new StringDictionary()),
      Server(new PersoServerConnection("PersoServerConnection")) {}

PersoServerAsyncWrapper::~PersoServerAsyncWrapper() {}

void PersoServerAsyncWrapper::connect() {
  initOperation("connectToServer");

  ReturnStatus ret;
  ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    processOperationError("connectToServer", ret);
    return;
  }

  completeOperation("connectToServer");
}

void PersoServerAsyncWrapper::disconnect() {
  initOperation("connectToServer");

  Server->disconnect();

  completeOperation("connectToServer");
}

void PersoServerAsyncWrapper::launchProductionLine(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("launchProductionLine");

  ReturnStatus ret;
  ret = Server->launchProductionLine(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("launchProductionLine", ret);
    return;
  }

  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit boxDataReady(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  completeOperation("launchProductionLine");
}

void PersoServerAsyncWrapper::shutdownProductionLine() {
  initOperation("shutdownProductionLine");

  Server->shutdownProductionLine();

  ProductionLineData->clear();
  emit boxDataReady(ProductionLineData);

  completeOperation("shutdownProductionLine");
}

void PersoServerAsyncWrapper::getProductionLineData() {
  initOperation("getProductionLineData");

  ReturnStatus ret;
  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit productionLineDataReady(ProductionLineData);
    processOperationError("getProductionLineData", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  completeOperation("getProductionLineData");
}

void PersoServerAsyncWrapper::logOn(const std::shared_ptr<StringDictionary> param) {
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

  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit boxDataReady(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  emit authorizationCompleted();
  completeOperation("logOnServer");
}

void PersoServerAsyncWrapper::logOut() {
  initOperation("logOutServer");

  if (Server->isConnected()) {
    Server->shutdownProductionLine();
    Server->disconnect();
  }

  completeOperation("logOutServer");
}

void PersoServerAsyncWrapper::echo() {
  initOperation("echoServer");

  ReturnStatus ret;
  ret = Server->echo();
  if (ret != ReturnStatus::NoError) {
    processOperationError("echoServer", ret);
    return;
  }

  completeOperation("echoServer");
}

void PersoServerAsyncWrapper::requestBox() {
  initOperation("requestBox");

  ReturnStatus ret;

  ret = Server->requestBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  ret = Server->getCurrentBoxData(*BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData->clear();
    emit boxDataReady(BoxData);
    processOperationError("requestBox", ret);
    return;
  }

  emit boxDataReady(BoxData);

  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit boxDataReady(ProductionLineData);
    processOperationError("requestBox", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  // Если в боксе есть собранные транспондеры
  if (BoxData->value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(*TransponderData);
    if (ret != ReturnStatus::NoError) {
      TransponderData->clear();
      emit transponderDataReady(TransponderData);
      processOperationError("requestBox", ret);
      return;
    }

    emit transponderDataReady(TransponderData);
  }

  completeOperation("requestBox");
}

void PersoServerAsyncWrapper::getCurrentBoxData() {
  initOperation("getCurrentBoxData");

  ReturnStatus ret = Server->getCurrentBoxData(*BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData->clear();
    emit boxDataReady(BoxData);
    processOperationError("getCurrentBoxData", ret);
    sendLog("Не удалось получить данные текущего бокса. ");
    return;
  }

  emit boxDataReady(BoxData);

  completeOperation("getCurrentBoxData");
}

void PersoServerAsyncWrapper::refundCurrentBox() {
  initOperation("refundCurrentBox");

  ReturnStatus ret;
  ret = Server->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("refundCurrentBox", ret);
    return;
  }

  BoxData->clear();
  emit boxDataReady(BoxData);

  TransponderData->clear();
  emit transponderDataReady(TransponderData);

  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit boxDataReady(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  completeOperation("refundCurrentBox");
}

void PersoServerAsyncWrapper::completeCurrentBox() {
  initOperation("completeCurrentBox");

  ReturnStatus ret;
  ret = Server->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("completeCurrentBox", ret);
    return;
  }

  BoxData->clear();
  emit boxDataReady(BoxData);

  TransponderData->clear();
  emit transponderDataReady(TransponderData);

  ret = Server->getProductionLineData(*ProductionLineData);
  if (ret != ReturnStatus::NoError) {
    ProductionLineData->clear();
    emit boxDataReady(ProductionLineData);
    processOperationError("logOnServer", ret);
    return;
  }

  emit productionLineDataReady(ProductionLineData);

  completeOperation("completeCurrentBox");
}

void PersoServerAsyncWrapper::releaseTransponder() {
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
  ret = Server->getCurrentBoxData(*BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData->clear();
    emit boxDataReady(BoxData);
    processOperationError("getCurrentBoxData", ret);
    return;
  }

  emit boxDataReady(BoxData);

  // Запрашиваем данные выпущенного транспондера
  ret = Server->getCurrentTransponderData(*TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData->clear();
    emit transponderDataReady(TransponderData);
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Данные выпускаемого транспондера получены."));

  emit transponderDataReady(TransponderData);

  // Печатаем стикер
  ret = ReturnStatus::StickerPrinterConnectionError;
  emit printTransponderSticker_signal(*TransponderData, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }
  sendLog(QString("Стикер распечатан."));

  completeOperation("releaseTransponder");
}

void PersoServerAsyncWrapper::rereleaseTransponder(
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
  ret = Server->getTransponderData(requestParam, *TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData->clear();
    emit transponderDataReady(TransponderData);
    processOperationError("releaseTransponder", ret);
    return;
  }

  // Запрашиваем отображение данных транспондера
  emit transponderDataReady(TransponderData);

  // Печатаем стикер
  emit printTransponderSticker_signal(*TransponderData, ret);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  completeOperation("rereleaseTransponder");
}

void PersoServerAsyncWrapper::rollbackTransponder() {
  initOperation("rollbackTransponder");
  sendLog("Откат производственной линии. ");

  ReturnStatus ret;
  ret = Server->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(*BoxData);
  if (ret != ReturnStatus::NoError) {
    BoxData->clear();
    emit boxDataReady(BoxData);
    processOperationError("rollbackTransponder", ret);
    return;
  }

  emit boxDataReady(BoxData);

  // Если в боксе есть собранные транспондеры
  if (BoxData->value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(*TransponderData);
    if (ret != ReturnStatus::NoError) {
      TransponderData->clear();
      emit transponderDataReady(TransponderData);
      processOperationError("rollbackTransponder", ret);
      return;
    }

    emit transponderDataReady(TransponderData);
  } else {
    TransponderData->clear();
    emit transponderDataReady(TransponderData);
  }

  completeOperation("rollbackTransponder");
}

void PersoServerAsyncWrapper::getCurrentTransponderData() {
  initOperation("getCurrentTransponderData");

  ReturnStatus ret;
  ret = Server->getCurrentTransponderData(*TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData->clear();
    emit transponderDataReady(TransponderData);
    processOperationError("getCurrentTransponderData", ret);
    return;
  }

  emit transponderDataReady(TransponderData);

  completeOperation("getCurrentTransponderData");
}

void PersoServerAsyncWrapper::getTransponderData(
    const std::shared_ptr<StringDictionary> param) {
  initOperation("getTransponderData");

  ReturnStatus ret;
  ret = Server->getTransponderData(*param, *TransponderData);
  if (ret != ReturnStatus::NoError) {
    TransponderData->clear();
    emit transponderDataReady(TransponderData);
    processOperationError("getTransponderData", ret);
    return;
  }

  emit transponderDataReady(TransponderData);

  completeOperation("getTransponderData");
}

void PersoServerAsyncWrapper::printBoxSticker(
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

void PersoServerAsyncWrapper::printLastBoxSticker() {
  initOperation("printLastBoxSticker");

  ReturnStatus ret;
  ret = Server->printLastBoxSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastBoxSticker", ret);
    return;
  }

  completeOperation("printLastBoxSticker");
}

void PersoServerAsyncWrapper::printPalletSticker(
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

void PersoServerAsyncWrapper::printLastPalletSticker() {
  initOperation("printLastPalletSticker");

  ReturnStatus ret;
  ret = Server->printLastPalletSticker();
  if (ret != ReturnStatus::NoError) {
    processOperationError("printLastPalletSticker", ret);
    return;
  }

  completeOperation("printLastPalletSticker");
}

void PersoServerAsyncWrapper::onServerDisconnected() {
  ProductionLineData->clear();
  emit productionLineDataReady(ProductionLineData);

  BoxData->clear();
  emit boxDataReady(BoxData);

  TransponderData->clear();
  emit transponderDataReady(TransponderData);
}

ReturnStatus PersoServerAsyncWrapper::checkConfig() {
  sendLog("Проверка конфигурации.");

  ReturnStatus ret = ReturnStatus::NoError;

  sendLog("Проверка конфигурации успешно завершена.");
  return ret;
}
