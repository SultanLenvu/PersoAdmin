#include "server_disconnected_signal.h"
#include "global_environment.h"
#include "server_connection_async_wrapper.h"

ServerDisconnectionSignal::ServerDisconnectionSignal() {
  const ServerConnectionAsyncWrapper* psaw =
      GlobalEnvironment::instance()
          ->getObject<const ServerConnectionAsyncWrapper>(
              "ServerConnectionAsyncWrapper");

  QObject::connect(this, &ServerDisconnectionSignal::signal, psaw,
                   &ServerConnectionAsyncWrapper::onServerDisconnected);
}
