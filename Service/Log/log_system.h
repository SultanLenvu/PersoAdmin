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
#include "configurable_object1.h"
#include "log_backend.h"

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public ConfigurableObject, public ConfigurableObject1 {
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
  void generate(const QString& log);

 private:
  virtual bool initInternals(void) override;
  virtual void loadSettings(void) override;

 private:
  Q_DISABLE_COPY_MOVE(LogSystem)
  void doLoadSettings(void);

  void createPersoServerLogSocket(void);

 private slots:
  void udpSocketReadyRead_slot();
};

//==================================================================================

#endif  // LOGSYSTEM_H
