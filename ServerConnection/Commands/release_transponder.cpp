#include "release_transponder.h"
ReleaseTransponder::ReleaseTransponder(const QString& name)
    : AbstractClientCommand(name) {}

ReleaseTransponder::~ReleaseTransponder() {}

const QString& ReleaseTransponder::name() {
  return Name;
}

ReturnStatus ReleaseTransponder::generate(const StringDictionary& param,
                                          QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus ReleaseTransponder::processResponse(
    const QByteArray& dataBlock,
    StringDictionary& responseData) {
  if (!processDataBlock(dataBlock)) {
    sendLog("Получена ошибка при обработке полученного блока данных.");
    return ReturnStatus::ServerResponseDataBlockError;
  }

  if (!Response.contains("return_status")) {
    return ReturnStatus::ServerResponseSyntaxError;
  }

  ReturnStatus ret = processReturnStatus(Response["return_status"].toString());
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при выполнении команды на сервере.");
    return ret;
  }

  if (!checkSyntax()) {
    return ReturnStatus::ServerResponseSyntaxError;
  }

  responseData["transponder_firmware"] =
      Response["transponder_firmware"].toString();

  return ReturnStatus::NoError;
}

bool ReleaseTransponder::checkSyntax() {
  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      !Response.contains("transponder_firmware")) {
    return false;
  }

  return true;
}
