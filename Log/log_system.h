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
#include "psobject.h"

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public PSObject {
  Q_OBJECT

 private:
  QString SavePath;
  std::vector<std::shared_ptr<LogBackend>> Backends;

  bool UdpListenEnable;
  //  std::unique_ptr<QUdpSocket> PersoServerLogSocket;
  // Из-за внутренних механизмов Qt лучше использовать "сырой" указатель
  QUdpSocket* PersoServerLogSocket;
  QHostAddress UdpListenIp;
  uint32_t UdpListenPort;

 public:
  LogSystem(const QString& name);
  ~LogSystem();

 public:  // PSObject interface
  virtual void applySettings(void) override;

 public slots:
  void instanceThreadStarted(void);

  void generate(const QString& log);

 private:
  Q_DISABLE_COPY_MOVE(LogSystem)
  void loadSettings(void);

  void createPersoServerLogSocket(void);

 private slots:
  void udpSocketReadyRead_slot();
};

//==================================================================================

#endif  // LOGSYSTEM_H
