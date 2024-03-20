#include "assembly_unit_manager.h"
#include "assembly_unit_gui_subkernel.h"
#include "global_environment.h"
#include "perso_server_connection.h"

AssemblyUnitManager::AssemblyUnitManager(const QString& name)
    : NamedObject(name), LoggableObject(name) {
  connectDependecies();
}

ReturnStatus AssemblyUnitManager::logOn(const StringDictionary& param) {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary pldata;

  ret = Server->connect();
  if (ret != ReturnStatus::NoError) {
    return ret;
  }

  ret = Server->launchProductionLine(param);
  if (ret != ReturnStatus::NoError) {
    sendLog("Не удалось запустить производственную линию.");
    return ret;
  }

  ret = Server->getProductionLineData(pldata);
  if (ret != ReturnStatus::NoError) {
    sendLog("Не удалось получит данные производственной линии.");
    return ret;
  }

  emit productionLineDataReady(pldata);
  emit authorizationCompleted();

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::logOut() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  if (Server->isConnected()) {
    Server->shutdownProductionLine();
    Server->disconnect();
  }

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::requestBox() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary bdata;
  StringDictionary pldata;
  StringDictionary tdata;

  ret = Server->requestBox();
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при запросе бокса.");
    return ret;
  }

  ret = Server->getCurrentBoxData(bdata);
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при запросе данных текущего бокса.");
    return ret;
  }

  emit boxDataReady(bdata);

  ret = Server->getProductionLineData(pldata);
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при запросе данных производственной линии.");
    return ret;
  }

  emit productionLineDataReady(pldata);

  // Если в боксе есть собранные транспондеры
  if (bdata.value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(tdata);
    if (ret != ReturnStatus::NoError) {
      sendLog("Получена ошибка при запросе данных текущего транспондера.");
      return ret;
    }

    emit transponderDataReady(tdata);
  }

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::refundCurrentBox() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary pldata;

  ret = Server->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при возврате текущего бокса.");
    return ret;
  }

  ret = Server->getProductionLineData(pldata);
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при получении данных производственной линии.");
    return ret;
  }

  emit productionLineDataReady(pldata);

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::completeCurrentBox() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary pldata;

  ret = Server->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при получении завершении сборки текущего бокса.");
    return ret;
  }

  ret = Server->getProductionLineData(pldata);
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при получении данных производственной линии.");
    return ret;
  }

  emit productionLineDataReady(pldata);

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::releaseTransponder() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary bdata;
  StringDictionary tdata;

  QString ucid;
  StringDictionary result;
  StringDictionary param;

  // Выпускаем транспондер
  ret = Server->releaseTransponder(result);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при выпуске транспондера."));
    return ret;
  }

  // Подтверждаем выпуск транспондера
  param.insert("transponder_ucid", ucid);
  ret = Server->confirmTransponderRelease(param);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при подтверждении выпуска транспондера."));
    return ret;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(bdata);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при запросе данных текущего бокса."));
    return ret;
  }

  emit boxDataReady(bdata);

  // Запрашиваем данные выпущенного транспондера
  ret = Server->getCurrentTransponderData(tdata);
  if (ret != ReturnStatus::NoError) {
    sendLog(
        QString("Получена ошибка при запросе данных текущего транспондера."));
    return ret;
  }

  emit transponderDataReady(tdata);

  // Печатаем стикер
  ret = ReturnStatus::StickerPrinterConnectionError;
  emit printTransponderSticker_signal(tdata, ret);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при печати стикер на транспондер."));
    return ret;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::rereleaseTransponder(
    const StringDictionary& param) {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary bdata;
  StringDictionary tdata;

  QString ucid;
  StringDictionary result;

  // Выпускаем транспондер
  ret = Server->rereleaseTransponder(param, result);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при перевыпуске транспондера."));
    return ret;
  }

  // Подтверждаем выпуск транспондера
  StringDictionary param2;
  param2.insert("transponder_ucid", ucid);
  param2.insert("transponder_ucid", param["pan"]);
  ret = Server->confirmTransponderRelease(param);
  if (ret != ReturnStatus::NoError) {
    sendLog(
        QString("Получена ошибка при подтверждении перевыпуска транспондера."));
    return ret;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(bdata);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при запросе данных текущего бокса."));
    return ret;
  }

  emit boxDataReady(bdata);

  // Запрашиваем данные выпущенного транспондера
  ret = Server->getCurrentTransponderData(tdata);
  if (ret != ReturnStatus::NoError) {
    sendLog(
        QString("Получена ошибка при запросе данных текущего транспондера."));
    return ret;
  }

  emit transponderDataReady(tdata);

  // Печатаем стикер
  ret = ReturnStatus::StickerPrinterConnectionError;
  emit printTransponderSticker_signal(tdata, ret);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при печати стикер на транспондер."));
    return ret;
  }

  return ReturnStatus::NoError;
}

ReturnStatus AssemblyUnitManager::rollbackTransponder() {
  if (!Server) {
    return ReturnStatus::ServerConnectionMissed;
  }

  ReturnStatus ret = ReturnStatus::NoError;
  StringDictionary bdata;
  StringDictionary tdata;

  ret = Server->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при откате текущего транспондер."));
    return ret;
  }

  // Обновляем данные бокса
  ret = Server->getCurrentBoxData(bdata);
  if (ret != ReturnStatus::NoError) {
    sendLog(QString("Получена ошибка при запросе данных текущего бокса."));
    return ret;
  }

  emit boxDataReady(bdata);

  // Если в боксе есть собранные транспондеры
  if (bdata.value("box_assembled_units").toInt() > 0) {
    ret = Server->getCurrentTransponderData(tdata);
    if (ret != ReturnStatus::NoError) {
      sendLog(
          QString("Получена ошибка при запросе данных текущего транспондера."));
      return ret;
    }

    emit transponderDataReady(tdata);
  }

  return ReturnStatus::NoError;
}

void AssemblyUnitManager::connectDependecies() {
  Server = std::static_pointer_cast<IServerConnection>(
      GlobalEnvironment::instance()->getSharedObject<PersoServerConnection>(
          "PersoServerConnection"));

  AssemblyUnitGuiSubkernel* augs =
      GlobalEnvironment::instance()->getObject<AssemblyUnitGuiSubkernel>(
          "AssemblyUnitGuiSubkernel");

  QObject::connect(this, &AssemblyUnitManager::productionLineDataReady, augs,
                   &AssemblyUnitGuiSubkernel::displayProductionLineData);
  QObject::connect(this, &AssemblyUnitManager::boxDataReady, augs,
                   &AssemblyUnitGuiSubkernel::displayBoxData);
  QObject::connect(this, &AssemblyUnitManager::transponderDataReady, augs,
                   &AssemblyUnitGuiSubkernel::displayTransponderData);
  QObject::connect(this, &AssemblyUnitManager::firwareReady, augs,
                   &AssemblyUnitGuiSubkernel::displayFirmware);
}
