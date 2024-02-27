#include "return_status_handler.h"

ReturnStatusHandler::ReturnStatusHandler(const QString& name)
    : AbstractReturnStatusHandler{name} {}

ReturnStatusHandler::~ReturnStatusHandler()
{
  
}


void ReturnStatusHandler::handle(ReturnStatus ret)
{
}
