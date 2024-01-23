#include "QNetworkDatagram"

#include "log_system.h"
#include "file_log_backend.h"
#include "global_environment.h"
#include "widget_log_backend.h"

LogSystem::LogSystem(const QString& name) : QObject(nullptr) {
  setObjectName(name);
  loadSettings();

  //  instanceThreadStarted();

  GlobalEnvironment::instance()->registerObject(this);
}

LogSystem::~LogSystem() {}

void LogSystem::instanceThreadStarted() {
  Backends.push_back(std::shared_ptr<WidgetLogBackend>(
      new WidgetLogBackend("WidgetLogBackend")));
  Backends.push_back(
      std::shared_ptr<FileLogBackend>(new FileLogBackend("FileLogBackend")));

  createPersoServerLogSocket();

  if (PersoServerLogSocket->thread() != QApplication::instance()->thread()) {
    qDebug() << "Запущен в отдельном потоке.";
  }
}

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

  PersoServerLogSocket->abort();
  PersoServerLogSocket->bind(UdpListenIp, UdpListenPort);

  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
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
}

void LogSystem::createPersoServerLogSocket() {
  PersoServerLogSocket = std::unique_ptr<QUdpSocket>(new QUdpSocket());
  //  PersoServerLogSocket = new QUdpSocket(this);
  PersoServerLogSocket->bind(UdpListenIp, UdpListenPort);
  connect(PersoServerLogSocket.get(), &QUdpSocket::readyRead, this,
          &LogSystem::udpSocketReadyRead_slot);
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
