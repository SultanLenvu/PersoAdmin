#include "refund_current_box.h"

RefundCurrentBox::RefundCurrentBox(const QString& name)
    : AbstractClientCommand(name) {}

RefundCurrentBox::~RefundCurrentBox() {}

const QString& RefundCurrentBox::name() {
  return Name;
}

ReturnStatus RefundCurrentBox::generate(const StringDictionary& param,
                                        QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus RefundCurrentBox::processResponse(const QByteArray& dataBlock,
                                               StringDictionary& responseData) {
  if (!processDataBlock(dataBlock)) {
    sendLog("Получена ошибка при обработке полученного блока данных.");
    return ReturnStatus::ServerResponseDataBlockError;
  }

  if (!checkSyntax()) {
    return ReturnStatus::ServerResponseSyntaxError;
  }

  ReturnStatus ret = processReturnStatus(Response["return_status"].toString());
  if (ret != ReturnStatus::NoError) {
    sendLog("Получена ошибка при выполнении команды на сервере.");
    return ret;
  }

  return ReturnStatus::NoError;
}

bool RefundCurrentBox::checkSyntax() {
  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      !Response.contains("return_status")) {
    return false;
  }

  return true;
}
