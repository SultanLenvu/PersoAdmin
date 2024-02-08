#include "complete_current_box.h"

CompleteCurrentBox::CompleteCurrentBox(const QString& name)
    : AbstractClientCommand(name) {}

CompleteCurrentBox::~CompleteCurrentBox() {}

const QString& CompleteCurrentBox::name() {
  return Name;
}

ReturnStatus CompleteCurrentBox::generate(
    const StringDictionary& param,
    QByteArray& dataBlock) {  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus CompleteCurrentBox::processResponse(
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
