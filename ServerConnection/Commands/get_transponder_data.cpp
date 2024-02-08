#include "get_transponder_data.h"

GetTransponderData::GetTransponderData(const QString& name)
    : AbstractClientCommand(name) {}

GetTransponderData::~GetTransponderData() {}

const QString& GetTransponderData::name() {
  return Name;
}

ReturnStatus GetTransponderData::generate(const StringDictionary& param,
                                          QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды
  Request["transponder_pan"] = param.value("transponder_pan");

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus GetTransponderData::processResponse(
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

  responseData["transponder_sn"] = Response["transponder_sn"].toString();
  responseData["transponder_pan"] = Response["transponder_pan"].toString();
  responseData["transponder_ucid"] = Response["transponder_ucid"].toString();
  responseData["transponder_release_counter"] =
      Response["transponder_release_counter"].toString();
  responseData["box_id"] = Response["box_id"].toString();
  responseData["issuer_name"] = Response["issuer_name"].toString();

  return ReturnStatus::NoError;
}

bool GetTransponderData::checkSyntax() {
  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      !Response.contains("transponder_sn") ||
      !Response.contains("transponder_pan") ||
      !Response.contains("transponder_ucid") ||
      !Response.contains("transponder_release_counter") ||
      !Response.contains("box_id") || !Response.contains("issuer_name")) {
    return false;
  }

  return true;
}
