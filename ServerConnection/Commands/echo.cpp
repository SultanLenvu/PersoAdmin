#include "echo.h"

Echo::Echo(const QString& name) : AbstractClientCommand(name) {}

Echo::~Echo() {}

const QString& Echo::name() {
  return Name;
}

ReturnStatus Echo::generate(const StringDictionary& param,
                            QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды
  Request["data"] = param.value("data");

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus Echo::processResponse(const QByteArray& dataBlock,
                                   StringDictionary& responseData) {
  if (!processDataBlock(dataBlock)) {
    return ReturnStatus::ServerResponseDataBlockError;
  }

  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      (!Response.contains("data"))) {
    sendLog("Получена синтаксическая ошибка при обработке серверного ответа.");
    return ReturnStatus::ServerResponseSyntaxError;
  }

  responseData["data"] = Response["data"].toString();

  return ReturnStatus::NoError;
}
