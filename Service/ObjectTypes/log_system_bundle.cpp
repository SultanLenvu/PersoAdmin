#include "log_system_bundle.h"
#include "global_environment.h"
#include "log_system.h"

LogSystemBundle::LogSystemBundle() : QObject{nullptr} {
  LogSystem* ls = static_cast<LogSystem*>(
      GlobalEnvironment::instance()->getObject("LogSystem"));

  connect(this, &LogSystemBundle::logging, ls, &LogSystem::generate);
}
