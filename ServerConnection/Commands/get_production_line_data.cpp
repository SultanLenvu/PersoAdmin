#include "get_production_line_data.h"

GetProductionLineData::GetProductionLineData(const QString& name)
    : AbstractClientCommand(name) {}

GetProductionLineData::~GetProductionLineData() {}

const QString& GetProductionLineData::name() {
  return Name;
}

ReturnStatus GetProductionLineData::generate(const StringDictionary& param,
                                             QByteArray& dataBlock) {
  Request["command_name"] = Name;

  // Тело команды
  generateDataBlock(dataBlock);
  return ReturnStatus::NoError;
}

ReturnStatus GetProductionLineData::processResponse(
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

  responseData["production_line_id"] =
      Response["production_line_id"].toString();
  responseData["production_line_login"] =
      Response["production_line_login"].toString();
  responseData["production_line_ns"] =
      Response["production_line_ns"].toString();
  responseData["production_line_in_process"] =
      Response["production_line_in_process"].toString();
  responseData["today_assembled_boxes"] =
      Response["today_assembled_boxes"].toString();
  responseData["transponder_id"] = Response["transponder_id"].toString();
  responseData["box_id"] = Response["box_id"].toString();

  return ReturnStatus::NoError;
}

bool GetProductionLineData::checkSyntax() {
  if ((Response.size() != ResponseSize) || (Response["command_name"] != Name) ||
      (!Response.contains("production_line_id")) ||
      (!Response.contains("production_line_login")) ||
      (!Response.contains("production_line_ns")) ||
      (!Response.contains("production_line_in_process") ||
       (!Response.contains("today_assembled_boxes")))) {
    return false;
  }

  return true;
}
