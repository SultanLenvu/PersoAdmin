#include "rerelease_transponder.h"

RereleaseTransponder::RereleaseTransponder(const QString& name)
    : AbstractClientCommand(name) {}

RereleaseTransponder::~RereleaseTransponder() {}

const QString& RereleaseTransponder::name() {
  return Name;
}

ReturnStatus RereleaseTransponder::generate(
    const StringDictionary& param,
    QByteArray& dataBlock) {  // Заголовок команды
  Request["command_name"] = Name;

  if (!param.contains("transponder_pan")) {
    return ReturnStatus::ParameterError;
  }

  // Тело команды
  Request["transponder_pan"] = param["transponder_pan"];

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus RereleaseTransponder::processResponse(
    const QByteArray& dataBlock,
    StringDictionary& responseData) {
  if (!processDataBlock(dataBlock)) {
    sendLog("Получена ошибка при обработке полученного блока данных.");
    return ReturnStatus::ServerResponseDataBlockError;
  }

  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      (!Response.contains("return_status")) ||
      (!Response.contains("transponder_firmware"))) {
    return ReturnStatus::ServerResponseSyntaxError;
  }

  responseData["transponder_firmware"] =
      Response["transponder_firmware"].toString();

  ReturnStatus ret = processReturnStatus(Response["return_status"].toString());
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при выполнении команды на сервере.");
    return ret;
  }

  return ReturnStatus::NoError;
}
