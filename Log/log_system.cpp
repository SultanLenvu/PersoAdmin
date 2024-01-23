#include "QNetworkDatagram"

#include "log_system.h"
#include "file_log_backend.h"
#include "global_environment.h"
#include "widget_log_backend.h"

LogSystem::LogSystem(const QString& name) : QObject(nullptr) {
  setObjectName(name);
  loadSettings();

  Backends.push_back(std::shared_ptr<WidgetLogBackend>(
      new WidgetLogBackend("WidgetLogBackend")));

  Backends.push_back(
      std::shared_ptr<FileLogBackend>(new FileLogBackend("FileLogBackend")));

<<<<<<< Updated upstream
  GlobalEnvironment::instance()->registerObject(this);
=======
  UdpSocket = new QUdpSocket(this);
  if (UdpListenEnable) {
    UdpSocket->bind(UdpListenIp, UdpListenPort);
  }
  connect(UdpSocket, &QUdpSocket::readyRead, this,
          &LogSystem::udpSocketReadyRead_slot);
>>>>>>> Stashed changes
}

LogSystem::~LogSystem() {}

void LogSystem::clear() {
  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
    (*it)->clear();
  }
}

void LogSystem::generate(const QString& log) {
  QTime time = QDateTime::currentDateTime().time();
  QString LogData = time.toString("hh:mm:ss.zzz - ") + log;
  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
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

<<<<<<< Updated upstream
  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
=======
  for (auto it = Backends.begin(); it != Backends.end(); it++) {
>>>>>>> Stashed changes
    (*it)->applySettings();
  }
}

/*
 * Приватные методы
 */

void LogSystem::loadSettings() {
  QSettings settings;

  UdpListenEnable = settings.value("log_system/udp_listen_enable").toBool();
  UdpListenIp = QHostAddress(
      settings.value("log_system/udp_listen_ip").toString());
  UdpListenPort = settings.value("log_system/udp_listen_port").toUInt();

  if (UdpListenEnable) {
    createUdpSocket();
  }
}

<<<<<<< Updated upstream
void LogSystem::createUdpSocket() {
  UdpSocket = std::unique_ptr<QUdpSocket>(new QUdpSocket());
  UdpSocket->bind(UdpListenIp, UdpListenPort);
  connect(UdpSocket.get(), &QUdpSocket::readyRead, this,
          &LogSystem::udpSocketReadyRead_slot);
}

=======
>>>>>>> Stashed changes
void LogSystem::udpSocketReadyRead_slot() {
  QByteArray datagram;
  datagram.resize(UdpSocket->pendingDatagramSize());

  UdpSocket->readDatagram(datagram.data(), datagram.size());
  generate(QString::fromUtf8(datagram));

  // Другой рабочий вариант
  //  QNetworkDatagram datagram;

  //  datagram = UdpSocket->receiveDatagram();

  //  generate(datagram.data());
}
