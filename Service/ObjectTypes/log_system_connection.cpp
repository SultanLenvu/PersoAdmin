#include "log_system_connection.h"
#include "global_environment.h"
#include "log_system.h"

LogSystemConnection::LogSystemConnection() : QObject{nullptr} {
  LogSystem* ls =
      GlobalEnvironment::instance()->getObject<LogSystem>("LogSystem");

  connect(this, &LogSystemConnection::logging, ls, &LogSystem::generate);
}
