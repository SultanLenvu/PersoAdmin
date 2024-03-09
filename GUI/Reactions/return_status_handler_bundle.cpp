#include "return_status_handler_bundle.h"
#include "global_environment.h"
#include "return_status_handler.h"

ReturnStatusHandlerBundle::ReturnStatusHandlerBundle() : QObject{nullptr} {
  ReturnStatusHandler* is = static_cast<ReturnStatusHandler*>(
      GlobalEnvironment::instance()->getObject("ReturnStatusHandler"));

  connect(this, &ReturnStatusHandlerBundle::handle_signal, is,
          &ReturnStatusHandler::handle);
}

void ReturnStatusHandlerBundle::handle(ReturnStatus ret) {
  emit handle_signal(ret);
}
