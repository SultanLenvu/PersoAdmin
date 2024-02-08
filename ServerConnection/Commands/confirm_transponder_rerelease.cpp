#include "confirm_transponder_rerelease.h"

ConfirmTransponderRerelease::ConfirmTransponderRerelease(const QString& name)
    : AbstractClientCommand(name) {}

ConfirmTransponderRerelease::~ConfirmTransponderRerelease() {}

const QString& ConfirmTransponderRerelease::name() {
  return Name;
}

ReturnStatus ConfirmTransponderRerelease::generate(const StringDictionary& param,
                                               QByteArray& dataBlock) {
  Request["command_name"] = Name;

  if (!param.contains("transponder_pan") ||
      !param.contains("transponder_ucid")) {
    return ReturnStatus::ParameterError;
  }

  // Тело команды
  Request["transponder_pan"] = param["transponder_pan"];
  Request["transponder_ucid"] = param["transponder_ucid"];

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus ConfirmTransponderRerelease::processResponse(
    const QByteArray& dataBlock,
    StringDictionary& responseData) {
  if (!processDataBlock(dataBlock)) {
    sendLog("Получена ошибка при обработке полученного блока данных.");
    return ReturnStatus::ServerResponseDataBlockError;
  }

  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      (!Response.contains("return_status"))) {
    return ReturnStatus::ServerResponseSyntaxError;
  }

  ReturnStatus ret = processReturnStatus(Response["return_status"].toString());
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при выполнении команды на сервере.");
    return ret;
  }

  return ReturnStatus::NoError;
}
