#include "request_box.h"

RequestBox::RequestBox(const QString& name) : AbstractClientCommand(name) {}

RequestBox::~RequestBox() {}

const QString& RequestBox::name() {
  return Name;
}

ReturnStatus RequestBox::generate(const StringDictionary& param,
                                  QByteArray& dataBlock) {
  Request["command_name"] = Name;

  // Тело команды

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus RequestBox::processResponse(const QByteArray& dataBlock,
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
