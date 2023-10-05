#include "log_system.h"

LogSystem::LogSystem(QObject* parent) : QObject(parent) {
  setObjectName("LogSystem");
  loadSettings();

  PersoServerLogSocket = new QUdpSocket(this);
  connect(PersoServerLogSocket, &QUdpSocket::readyRead, this,
          &LogSystem::on_PersoServerLogSocketReadyRead_slot);
}

LogSystem::~LogSystem() {}

void LogSystem::clear() {
  if (!GlobalEnableOption) {
    return;
  }

  emit requestClearDisplayLog();
}

void LogSystem::setEnable(bool option) {
  GlobalEnableOption = option;
}

void LogSystem::applySettings() {}

void LogSystem::generate(const QString& log) {
  if (!GlobalEnableOption) {
    return;
  }

  QTime time = QDateTime::currentDateTime().time();
  QString LogData = time.toString("hh:mm:ss.zzz - ") + log;
  emit requestDisplayLog(LogData);
}

/*
 * Приватные методы
 */

void LogSystem::loadSettings() {
  QSettings settings;

  GlobalEnableOption = settings.value("LogSystem/Enable").toBool();
  PersoServerLogEnable =
      settings.value("LogSystem/PersoServer/Enable").toBool();
  PersoServerLogAddress = settings.value("LogSystem/PersoServer/Ip").toString();
  PersoServerLogPort = settings.value("LogSystem/PersoServer/Port").toInt();
}

void LogSystem::startListeningPersoServerLog() {
  generate("LogSystem - Start listening PersoServer logs. ");
  PersoServerLogSocket->bind(PersoServerLogAddress, PersoServerLogPort);
}

void LogSystem::stopListeningPersoServerLog() {
  generate("LogSystem - Stop listening PersoServer logs. ");
  PersoServerLogSocket->close();
}

void LogSystem::on_PersoServerLogSocketReadyRead_slot() {
  QByteArray datagram;
  datagram.resize(PersoServerLogSocket->pendingDatagramSize());
  PersoServerLogSocket->readDatagram(datagram.data(), datagram.size());
  generate("PersoServer - " + datagram);
}
