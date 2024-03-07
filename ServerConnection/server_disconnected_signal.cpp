#include "server_disconnected_signal.h"
#include "global_environment.h"
#include "perso_server_async_wrapper.h"

ServerDisconnectionSignal::ServerDisconnectionSignal() {
  PersoServerAsyncWrapper* psaw = static_cast<PersoServerAsyncWrapper*>(
      GlobalEnvironment::instance()->getObject("PersoServerAsyncWrapper"));

  QObject::connect(this, &ServerDisconnectionSignal::signal, psaw,
                   &PersoServerAsyncWrapper::onServerDisconnected);
}
