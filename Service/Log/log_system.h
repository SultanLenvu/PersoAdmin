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

#include "configurable_object.h"
#include "log_backend.h"

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public ConfigurableObject {
  Q_OBJECT

 private:
  QString SavePath;
  std::vector<std::unique_ptr<LogBackend>> Backends;

  bool UdpListenEnable;
  //  std::unique_ptr<QUdpSocket> PersoServerLogSocket;
  // Из-за внутренних механизмов Qt лучше использовать "сырой" указатель
  QUdpSocket* PersoServerLogSocket;
  QHostAddress UdpListenIp;
  uint32_t UdpListenPort;

 public:
  LogSystem(const QString& name);
  ~LogSystem();

 public slots:
  void onInstanceThreadStarted(void);

  void generate(const QString& log);

 private:
  Q_DISABLE_COPY_MOVE(LogSystem)
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

  void createPersoServerLogSocket(void);

 private slots:
  void udpSocketReadyRead_slot();
};

//==================================================================================

#endif  // LOGSYSTEM_H
