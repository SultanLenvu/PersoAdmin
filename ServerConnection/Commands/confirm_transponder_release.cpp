#include "confirm_transponder_release.h"

ConfirmTransponderRelease::ConfirmTransponderRelease(const QString& name)
    : AbstractClientCommand(name) {}

ConfirmTransponderRelease::~ConfirmTransponderRelease() {}

const QString& ConfirmTransponderRelease::name() {
  return Name;
}

ReturnStatus ConfirmTransponderRelease::generate(const StringDictionary& param,
                                                 QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды
  if (!param.contains("transponder_ucid")) {
    return ReturnStatus::ParameterError;
  }
  Request["transponder_ucid"] = param.value("transponder_ucid");

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus ConfirmTransponderRelease::processResponse(
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
