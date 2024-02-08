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
  QObject* ls = GlobalEnvironment::instance()->getObject("LogSystem");
  assert(ls);
  QObject* gk = GlobalEnvironment::instance()->getObject("GuiKernel");
  assert(gk);

  QObject::connect(this, &AbstractServerConnection::logging,
                   dynamic_cast<LogSystem*>(ls), &LogSystem::generate);
  //  QObject::connect(this, &AbstractServerConnection::disconnected,
  //                   dynamic_cast<GuiKernel*>(gk),
  //                   &GuiKernel::onServerDisconnected);
}
