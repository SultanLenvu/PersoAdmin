#include "abstract_server_connection.h"
#include "global_environment.h"
// #include "gui_kernel.h"
#include "log_system.h"

AbstractServerConnection::AbstractServerConnection(const QString& name)
    : QObject{nullptr} {
  setObjectName(name);

  connectDepedencies();
}

AbstractServerConnection::~AbstractServerConnection() {}

AbstractServerConnection::AbstractServerConnection() : QObject{nullptr} {}

void AbstractServerConnection::connectDepedencies() {
  LogSystem* ls = static_cast<LogSystem*>(
      GlobalEnvironment::instance()->getObject("LogSystem"));

  QObject::connect(this, &AbstractServerConnection::logging, ls,
                   &LogSystem::generate);
}
