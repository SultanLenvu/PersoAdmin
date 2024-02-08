#include "QNetworkDatagram"

#include "log_system.h"
#include "file_log_backend.h"
#include "global_environment.h"
#include "widget_log_backend.h"

LogSystem::LogSystem(const QString& name) : PSObject(name) {
  loadSettings();

  GlobalEnvironment::instance()->registerObject(this);
}

LogSystem::~LogSystem() {
  /*
   * Если удалять напрямую, то ловим критическую ошибку
   * из-за внутренних механизмов Qt
   */
  PersoServerLogSocket->deleteLater();

  //  PersoServerLogSocket->blockSignals(true);
  //  delete PersoServerLogSocket;
}

void LogSystem::applySettings() {
  generate("LogSystem - Применение новых настроек. ");
  loadSettings();

  PersoServerLogSocket->abort();
  PersoServerLogSocket->bind(UdpListenIp, UdpListenPort);

  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
    (*it)->applySettings();
  }
}

void LogSystem::instanceThreadStarted() {
  Backends.push_back(std::shared_ptr<WidgetLogBackend>(
      new WidgetLogBackend("WidgetLogBackend")));
  Backends.push_back(
      std::shared_ptr<FileLogBackend>(new FileLogBackend("FileLogBackend")));

  createPersoServerLogSocket();
}

void LogSystem::generate(const QString& log) {
  QTime time = QDateTime::currentDateTime().time();

  QString LogMessage =
      QString("%1 - %2").arg(time.toString("hh:mm:ss.zzz"), log);

  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
    (*it)->writeLogMessage(LogMessage);
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
}

void LogSystem::createPersoServerLogSocket() {
  //  PersoServerLogSocket = std::unique_ptr<QUdpSocket>(new QUdpSocket());
  //  connect(PersoServerLogSocket.get(), &QUdpSocket::readyRead, this,
  //          &LogSystem::udpSocketReadyRead_slot);
  PersoServerLogSocket = new QUdpSocket(nullptr);
  connect(PersoServerLogSocket, &QUdpSocket::readyRead, this,
          &LogSystem::udpSocketReadyRead_slot);
  PersoServerLogSocket->bind(UdpListenIp, UdpListenPort);
}

void LogSystem::udpSocketReadyRead_slot() {
  QByteArray datagram;
  datagram.resize(PersoServerLogSocket->pendingDatagramSize());

  PersoServerLogSocket->readDatagram(datagram.data(), datagram.size());

  // Другой рабочий вариант
  //  QNetworkDatagram datagram;
  //  datagram = PersoServerLogSocket->receiveDatagram();
  //  generate(datagram.data());

  if (UdpListenEnable) {
    generate(QString::fromUtf8(datagram));
  }
}
