#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include <QHostAddress>
#include <QObject>
#include <QSettings>
#include <QThread>
#include <QTime>
#include <QUdpSocket>

/* Глобальная система логгирования */
//==================================================================================

class LogSystem : public QObject {
  Q_OBJECT

 private:
  bool GlobalEnableOption;

  bool PersoServerLogEnable;
  uint32_t PersoServerLogPort;
  QHostAddress PersoServerLogAddress;
  QUdpSocket* PersoServerLogSocket;

 public:
  LogSystem(QObject* parent);
  ~LogSystem();

 public slots:
  void clear(void);
  void setEnable(bool option);
  void applySettings(void);
  void generate(const QString& log);

 private:
  Q_DISABLE_COPY(LogSystem)
  void loadSettings(void);
  void startListeningPersoServerLog(void);
  void stopListeningPersoServerLog(void);

 private slots:
  void on_PersoServerLogSocketReadyRead_slot(void);

 signals:
  void requestDisplayLog(const QString& logData);
  void requestClearDisplayLog(void);
};

//==================================================================================

#endif  // LOGSYSTEM_H
