#include "server_disconnected_signal.h"
#include "global_environment.h"
#include "server_connection_async_wrapper.h"

ServerDisconnectionSignal::ServerDisconnectionSignal() {
  ServerConnectionAsyncWrapper* psaw = static_cast<ServerConnectionAsyncWrapper*>(
      GlobalEnvironment::instance()->getObject("ServerConnectionAsyncWrapper"));

  QObject::connect(this, &ServerDisconnectionSignal::signal, psaw,
                   &ServerConnectionAsyncWrapper::onServerDisconnected);
}
