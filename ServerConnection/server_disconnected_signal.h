#ifndef SERVERDISCONNECTEDSIGNAL_H
#define SERVERDISCONNECTEDSIGNAL_H

#include <QObject>

class ServerDisconnectionSignal : public QObject {
  Q_OBJECT
 public:
  explicit ServerDisconnectionSignal();
  ~ServerDisconnectionSignal() = default;

 signals:
  void signal(void);
};

#endif // SERVERDISCONNECTEDSIGNAL_H
