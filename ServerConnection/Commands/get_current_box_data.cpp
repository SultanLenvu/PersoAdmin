#include "get_current_box_data.h"

GetCurrentBoxData::GetCurrentBoxData(const QString& name)
    : AbstractClientCommand(name) {}

GetCurrentBoxData::~GetCurrentBoxData() {}

const QString& GetCurrentBoxData::name() {
  return Name;
}

ReturnStatus GetCurrentBoxData::generate(const StringDictionary& param,
                                         QByteArray& dataBlock) {
  // Заголовок команды
  Request["command_name"] = Name;

  // Тело команды

  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus GetCurrentBoxData::processResponse(
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

  responseData["box_id"] = Response["box_id"].toString();
  responseData["box_in_process"] = Response["box_in_process"].toString();
  responseData["box_quantity"] = Response["box_quantity"].toString();
  responseData["box_assembled_units"] =
      Response["box_assembled_units"].toString();

  responseData["box_assembling_start"] =
      Response["box_assembling_start"].toString();
  responseData["box_assembling_end"] =
      Response["box_assembling_end"].toString();

  responseData["first_transponder_sn"] =
      Response["first_transponder_sn"].toString();
  responseData["last_transponder_sn"] =
      Response["last_transponder_sn"].toString();

  responseData["pallet_id"] = Response["pallet_id"].toString();
  responseData["production_line_id"] =
      Response["production_line_id"].toString();

  return ReturnStatus::NoError;
}

bool GetCurrentBoxData::checkSyntax() {
  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      !Response.contains("box_id") || !Response.contains("box_in_process") ||
      !Response.contains("box_quantity") ||
      !Response.contains("box_assembled_units") ||
      !Response.contains("box_assembling_start") ||
      !Response.contains("box_assembling_end") ||
      !Response.contains("first_transponder_sn") ||
      !Response.contains("last_transponder_sn") ||
      !Response.contains("pallet_id") ||
      !Response.contains("production_line_id")) {
    return false;
  }

  return true;
}
