#include "log_system_connection.h"
#include "global_environment.h"
#include "log_system.h"

LogSystemConnection::LogSystemConnection() : QObject{nullptr} {
  LogSystem* ls = static_cast<LogSystem*>(
      GlobalEnvironment::instance()->getObject("LogSystem"));

  connect(this, &LogSystemConnection::logging, ls, &LogSystem::generate);
}
