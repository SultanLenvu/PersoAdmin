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

#include "abstract_log_backend.h"
#include "configurable_object.h"
#include "loggable_object.h"
#include "named_object.h"

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public NamedObject,
                  public ConfigurableObject,
                  public LoggableObject {
  Q_OBJECT

 private:
  QString SavePath;
  std::vector<std::unique_ptr<AbstractLogBackend>> Backends;

  bool UdpListenEnable;
  //  std::unique_ptr<QUdpSocket> PersoServerLogSocket;
  // Из-за внутренних механизмов Qt лучше использовать "сырой" указатель
  QUdpSocket* PersoServerLogSocket;
  QHostAddress UdpListenIp;
  uint32_t UdpListenPort;

 public:
  Q_INVOKABLE LogSystem(const QString& name);
  ~LogSystem();

 public slots:
  void generate(const QString& log, const LoggableObject* source) const;

 private:
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
