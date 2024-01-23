#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include <QHostAddress>
#include <QList>
#include <QMutex>
#include <QObject>
#include <QSettings>
#include <QThread>
#include <QTime>
#include <QUdpSocket>

#include "log_backend.h"

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public QObject {
  Q_OBJECT

 private:
  QString SavePath;
  std::vector<std::shared_ptr<LogBackend>> Backends;

  bool UdpListenEnable;
  std::unique_ptr<QUdpSocket> PersoServerLogSocket;
  QHostAddress UdpListenIp;
  uint32_t UdpListenPort;

 public:
  LogSystem(const QString& name);
  ~LogSystem();

 public slots:
  void instanceThreadStarted(void);

  void clear(void);
  void generate(const QString& log);

  void applySettings(void);

 private:
  Q_DISABLE_COPY_MOVE(LogSystem)
  void loadSettings(void);

  void createPersoServerLogSocket(void);

 private slots:
  void udpSocketReadyRead_slot();
};

//==================================================================================

#endif  // LOGSYSTEM_H
