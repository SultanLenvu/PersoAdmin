#include "log_system.h"

#include <QNetworkDatagram>

#include "file_log_backend.h"
#include "widget_log_backend.h"

LogSystem::LogSystem(const QString& name)
    : NamedObject(name), LoggableObject(name) {
  doLoadSettings();

  Backends.emplace_back(new WidgetLogBackend("WidgetLogBackend"));
  Backends.emplace_back(new FileLogBackend("FileLogBackend"));

  createPersoServerLogSocket();
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

void LogSystem::generate(const QString& log,
                         const LoggableObject* source) const {
  QTime time = QDateTime::currentDateTime().time();

  QString LogMessage = QString("%1 - 0x%2 - %3 - %4")
                           .arg(time.toString("hh:mm:ss.zzz"),
                                QString::number(source->threadId(), 16),
                                source->sourceName(), log);

  for (auto it = Backends.begin(); it != Backends.end(); ++it) {
    (*it)->writeMessage(LogMessage);
  }
}

/*
 * Приватные методы
 */

void LogSystem::loadSettings() {
  sendLog("Загрузка настроек.");
  doLoadSettings();

  PersoServerLogSocket->abort();
  PersoServerLogSocket->bind(UdpListenIp, UdpListenPort);
}

void LogSystem::doLoadSettings() {
  QSettings settings;

  UdpListenEnable = settings.value("log_system/udp_listen_enable").toBool();
  UdpListenIp =
      QHostAddress(settings.value("log_system/udp_listen_ip").toString());
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
    generate(QString::fromUtf8(datagram), this);
  }
}
