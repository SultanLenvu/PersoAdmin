#ifndef SERVERDISCONNECTEDSIGNAL_H
#define SERVERDISCONNECTEDSIGNAL_H

#include <QObject>

class ServerDisconnectionSignal final : public QObject {
  Q_OBJECT
 public:
  explicit ServerDisconnectionSignal();
  ~ServerDisconnectionSignal() = default;

 private:
  Q_DISABLE_COPY_MOVE(ServerDisconnectionSignal)

 signals:
  void signal(void);
};

#endif // SERVERDISCONNECTEDSIGNAL_H
