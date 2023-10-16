#include "log_system.h"

LogSystem::LogSystem(QObject* parent) : QObject(parent) {
  setObjectName("LogSystem");
  loadSettings();

  WidgetLogger = new WidgetLogBackend(this);
  Backends << WidgetLogger;

  FileLogger = new FileLogBackend(this);
  Backends << FileLogger;

  UdpSocket = new QUdpSocket(this);
  if (UdpListenEnable) {
    UdpSocket->bind(UdpListenIp, UdpListenPort);
  }
  connect(UdpSocket, &QUdpSocket::readyRead, this,
          &LogSystem::on_UdpSocketReadyRead_slot);
}

LogSystem::~LogSystem() {}

WidgetLogBackend* LogSystem::getWidgetLogger() {
  return WidgetLogger;
}

LogSystem* LogSystem::instance() {
  static LogSystem Logger(nullptr);
  return &Logger;
}

void LogSystem::clear() {
  for (QList<LogBackend*>::iterator it = Backends.begin(); it != Backends.end();
       it++) {
    (*it)->clear();
  }
}

void LogSystem::generate(const QString& log) {
  QTime time = QDateTime::currentDateTime().time();
  QString LogData = time.toString("hh:mm:ss.zzz - ") + log;
  for (QList<LogBackend*>::const_iterator it = Backends.begin();
       it != Backends.end(); it++) {
    (*it)->writeLogLine(LogData);
  }
}

void LogSystem::applySettings() {
  generate("LogSystem - Применение новых настроек. ");
  loadSettings();

  UdpSocket->abort();
  if (UdpListenEnable) {
    UdpSocket->bind(UdpListenIp, UdpListenPort);
  }

  for (QList<LogBackend*>::const_iterator it = Backends.begin();
       it != Backends.end(); it++) {
    (*it)->applySettings();
  }
}

/*
 * Приватные методы
 */

void LogSystem::loadSettings() {
  QSettings settings;

  UdpListenEnable = settings.value("log_system/udp_listen_enable").toBool();
  UdpListenIp = settings.value("log_system/udp_listen_ip").toString();
  UdpListenPort = settings.value("log_system/udp_listen_port").toUInt();
}

void LogSystem::on_UdpSocketReadyRead_slot() {
  QByteArray datagram;
  datagram.resize(UdpSocket->bytesAvailable());

  UdpSocket->readDatagram(datagram.data(), datagram.size());
  generate(datagram);
}
