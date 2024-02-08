#include <QDataStream>
#include <QSettings>

#include "complete_current_box.h"
#include "confirm_transponder_release.h"
#include "confirm_transponder_rerelease.h"
#include "definitions.h"
#include "echo.h"
#include "get_current_box_data.h"
#include "get_current_transponder_data.h"
#include "get_production_line_data.h"
#include "get_transponder_data.h"
#include "launch_production_line.h"
#include "perso_server_connection.h"
#include "print_box_sticker.h"
#include "print_last_box_sticker.h"
#include "print_last_pallet_sticker.h"
#include "print_pallet_sticker.h"
#include "refund_current_box.h"
#include "release_transponder.h"
#include "request_box.h"
#include "rerelease_transponder.h"
#include "rollback_transponder.h"
#include "shutdown_production_line.h"

PersoServerConnection::PersoServerConnection(const QString& name)
    : AbstractServerConnection(name) {
  ReceivedDataBlockSize = 0;

  loadSettings();

  // Создаем сокет
  createSocket();

  // Создаем таймеры
  createTimers();

  // Создаем команды
  createCommands();
}

PersoServerConnection::~PersoServerConnection() {
  if (Socket->isOpen()) {
    Socket->disconnectFromHost();
  }
}

ReturnStatus PersoServerConnection::connect() {
  if (Socket->state() == QTcpSocket::ConnectedState) {
    sendLog("Соединение с сервером персонализации уже установлено. ");
    return ReturnStatus::NoError;
  }

  sendLog("Подключение к серверу персонализации. ");
  // Подключаемся к серверу персонализации
  Socket->connectToHost(PersoServerAddress, PersoServerPort);

  // Ожидаем подключения или отказа
  sendLog("Ожидание ответа от сервера. ");
  WaitTimer->start();
  WaitingLoop->exec();

  // Если соединение не удалось установить
  if (!Socket->isOpen()) {
    sendLog(
        "Не удалось установить соединение с сервером персонализации. Сброс. ");
    return ReturnStatus::ServerConnectionError;
  }

  return ReturnStatus::NoError;
}

void PersoServerConnection::disconnect() {
  sendLog("Отключение от сервера персонализации. ");
  Socket->disconnectFromHost();
}

bool PersoServerConnection::isConnected() {
  return Socket->isOpen();
}

ReturnStatus PersoServerConnection::echo() {
  CurrentCommand = Commands.at(Echo);

  StringDictionary param, result;
  param.insert("data", "test");
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::launchProductionLine(const StringDictionary& param) {
  CurrentCommand = Commands.at(LaunchProductionLine);

  StringDictionary result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::shutdownProductionLine() {
  CurrentCommand = Commands.at(ShutdownProductionLine);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::getProductionLineData(
    StringDictionary& data) {
  CurrentCommand = Commands.at(GetProductionLineData);

  StringDictionary param;
  ReturnStatus ret = processCurrentCommand(param, data);

  return ret;
}

ReturnStatus PersoServerConnection::requestBox() {
  CurrentCommand = Commands.at(RequestBox);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::getCurrentBoxData(
    StringDictionary& result) {
  CurrentCommand = Commands.at(GetCurrentBoxData);

  StringDictionary param;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::completeCurrentBox() {
  CurrentCommand = Commands.at(CompleteCurrentBox);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::refundCurrentBox() {
  CurrentCommand = Commands.at(RefundCurrentBox);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::getCurrentTransponderData(
    StringDictionary& result) {
  CurrentCommand = Commands.at(GetCurrentTransponderData);

  StringDictionary param;
  ReturnStatus ret = processCurrentCommand(param, result);
  return ret;
}

ReturnStatus PersoServerConnection::getTransponderData(
    const StringDictionary& param,
    StringDictionary& result) {
  CurrentCommand = Commands.at(GetTransponderData);

  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::releaseTransponder(
    StringDictionary& result) {
  CurrentCommand = Commands.at(ReleaseTransponder);

  StringDictionary param;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::confirmTransponderRelease(
    const StringDictionary& param) {
  CurrentCommand = Commands.at(ConfirmTransponderRelease);

  StringDictionary result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::rereleaseTransponder(
    const StringDictionary& param,
    StringDictionary& result) {
  CurrentCommand = Commands.at(RereleaseTransponder);

  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::confirmTransponderRerelease(
    const StringDictionary& param) {
  CurrentCommand = Commands.at(ConfirmTransponderRerelease);

  StringDictionary result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::rollbackTransponder() {
  CurrentCommand = Commands.at(RollbackTransponder);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::printBoxSticker(
    const StringDictionary& param) {
  CurrentCommand = Commands.at(PrintBoxSticker);

  StringDictionary result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::printLastBoxSticker() {
  CurrentCommand = Commands.at(PrintLastBoxSticker);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::printPalletSticker(
    const StringDictionary& param) {
  CurrentCommand = Commands.at(PrintPalletSticker);

  StringDictionary result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

ReturnStatus PersoServerConnection::printLastPalletSticker() {
  CurrentCommand = Commands.at(PrintLastPalletSticker);

  StringDictionary param, result;
  ReturnStatus ret = processCurrentCommand(param, result);

  return ret;
}

void PersoServerConnection::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
}

void PersoServerConnection::loadSettings() {
  QSettings settings;

  PersoServerAddress =
      QHostAddress(settings.value("perso_server_connection/ip").toString());
  PersoServerPort = settings.value("perso_server_connection/port").toInt();
}

void PersoServerConnection::sendLog(const QString& log) {
  emit logging(objectName() + " - " + log);
}

ReturnStatus PersoServerConnection::processCurrentCommand(
    const StringDictionary& param,
    StringDictionary& result) {
  sendLog(
      QString("Начало выполнения команды '%1'.").arg(CurrentCommand->name()));

  // Очистка
  clearBuffer();
  CurrentCommand->clear();

  QByteArray dataBlock;
  ReturnStatus ret = CurrentCommand->generate(param, dataBlock);
  if (ret != ReturnStatus::NoError) {
    return ret;
  }

  ret = transmitDataBlock(dataBlock);
  if (ret != ReturnStatus::NoError) {
    return ret;
  }

  if (!waitResponse()) {
    return ReturnStatus::ServerNotResponding;
  }

  ret = CurrentCommand->processResponse(ReceivedDataBlock, result);
  if (ret != ReturnStatus::NoError) {
    return ret;
  }

  sendLog(
      QString("Команда '%1' успешно выполнена.").arg(CurrentCommand->name()));
  return ReturnStatus::NoError;
}

ReturnStatus PersoServerConnection::transmitDataBlock(
    const QByteArray& dataBlock) {
  ReturnStatus ret;
  // Проверяем подключение к серверу персонализации
  if (!Socket->isOpen()) {
    sendLog("Соединение с сервером персонализации не установлено.");
    return ReturnStatus::ServerConnectionError;
  }

  // Если размер блок не превышает максимального размера данных для единоразовой
  // передачи
  if (dataBlock.size() < ONETIME_TRANSMIT_DATA_SIZE) {
    // Отправляем блок данных
    if (Socket->write(dataBlock) == -1) {
      sendLog("Получена ошибка при отправке блока данных.");
      return ReturnStatus::ServerDataTransmittingError;
    }

  } else {
    // В противном случае дробим блок данных на части и последовательно
    // отправляем
    for (int32_t i = 0; i < dataBlock.size(); i += ONETIME_TRANSMIT_DATA_SIZE) {
      if (Socket->write(dataBlock.mid(i, ONETIME_TRANSMIT_DATA_SIZE)) == -1) {
        sendLog(QString("Получена ошибка при отправке %1 части блока данных.")
                    .arg(QString::number(i)));
        return ReturnStatus::ServerDataTransmittingError;
      }
    }
  }

  return ReturnStatus::NoError;
}

bool PersoServerConnection::waitResponse() {
  // Ожидаем ответ
  WaitTimer->start();
  WaitingLoop->exec();

  // Если сервер ничего не ответил
  if (ReceivedDataBlock.isEmpty()) {
    sendLog("Сервер не отвечает.");
    return false;
  }

  return true;
}

void PersoServerConnection::createTimers() {
  WaitTimer = std::unique_ptr<QTimer>(new QTimer());
  WaitTimer->setInterval(PERSO_SERVER_CONNECTION_WAITING_TIME);
  QObject::connect(WaitTimer.get(), &QTimer::timeout, this,
                   &PersoServerConnection::waitTimerTimeout_slot);
  QObject::connect(WaitTimer.get(), &QTimer::timeout, WaitTimer.get(),
                   &QTimer::stop);
  QObject::connect(Socket.get(), &QTcpSocket::readyRead, WaitTimer.get(),
                   &QTimer::stop);
  QObject::connect(Socket.get(), &QTcpSocket::connected, WaitTimer.get(),
                   &QTimer::stop);
  QObject::connect(Socket.get(), &QTcpSocket::disconnected, WaitTimer.get(),
                   &QTimer::stop);
  QObject::connect(Socket.get(), &QTcpSocket::errorOccurred, WaitTimer.get(),
                   &QTimer::stop);

  WaitingLoop = std::unique_ptr<QEventLoop>(new QEventLoop());
  QObject::connect(Socket.get(), &QTcpSocket::connected, WaitingLoop.get(),
                   &QEventLoop::quit);
  QObject::connect(Socket.get(), &QTcpSocket::disconnected, WaitingLoop.get(),
                   &QEventLoop::quit);
  QObject::connect(Socket.get(), &QTcpSocket::errorOccurred, WaitingLoop.get(),
                   &QEventLoop::quit);
  QObject::connect(WaitTimer.get(), &QTimer::timeout, WaitingLoop.get(),
                   &QEventLoop::quit);
  QObject::connect(this, &PersoServerConnection::stopResponseWaiting,
                   WaitingLoop.get(), &QEventLoop::quit);
}

void PersoServerConnection::createSocket(void) {
  Socket = std::unique_ptr<QTcpSocket>(new QTcpSocket());
  QObject::connect(Socket.get(), &QTcpSocket::connected, this,
                   &PersoServerConnection::socketConnected_slot);
  QObject::connect(Socket.get(), &QTcpSocket::disconnected, this,
                   &PersoServerConnection::socketDisconnected_slot);
  QObject::connect(Socket.get(), &QTcpSocket::readyRead, this,
                   &PersoServerConnection::socketReadyRead_slot);
  QObject::connect(Socket.get(), &QTcpSocket::errorOccurred, this,
                   &PersoServerConnection::socketError_slot);
}

void PersoServerConnection::createCommands() {
  Commands[Echo] = std::shared_ptr<::AbstractClientCommand>(new ::Echo("Echo"));
  Commands[LaunchProductionLine] =
      std::shared_ptr<::AbstractClientCommand>(new ::LaunchProductionLine("LaunchProductionLine"));
  Commands[ShutdownProductionLine] = std::shared_ptr<::AbstractClientCommand>(
      new ::ShutdownProductionLine("ShutdownProductionLine"));
  Commands[GetProductionLineData] = std::shared_ptr<::AbstractClientCommand>(
      new ::GetProductionLineData("GetProductionLineData"));

  Commands[RequestBox] =
      std::shared_ptr<::AbstractClientCommand>(new ::RequestBox("RequestBox"));
  Commands[GetCurrentBoxData] = std::shared_ptr<::AbstractClientCommand>(
      new ::GetCurrentBoxData("GetCurrentBoxData"));
  Commands[CompleteCurrentBox] = std::shared_ptr<::AbstractClientCommand>(
      new ::CompleteCurrentBox("CompleteCurrentBox"));
  Commands[RefundCurrentBox] = std::shared_ptr<::AbstractClientCommand>(
      new ::RefundCurrentBox("RefundCurrentBox"));

  Commands[GetCurrentTransponderData] =
      std::shared_ptr<::AbstractClientCommand>(
          new ::GetCurrentTransponderData("GetCurrentTransponderData"));
  Commands[GetTransponderData] = std::shared_ptr<::AbstractClientCommand>(
      new ::GetTransponderData("GetTransponderData"));

  Commands[ReleaseTransponder] = std::shared_ptr<::AbstractClientCommand>(
      new ::ReleaseTransponder("ReleaseTransponder"));
  Commands[ConfirmTransponderRelease] =
      std::shared_ptr<::AbstractClientCommand>(
          new ::ConfirmTransponderRelease("ConfirmTransponderRelease"));
  Commands[RereleaseTransponder] = std::shared_ptr<::AbstractClientCommand>(
      new ::RereleaseTransponder("RereleaseTransponder"));
  Commands[ConfirmTransponderRerelease] =
      std::shared_ptr<::AbstractClientCommand>(
          new ::ConfirmTransponderRerelease("ConfirmTransponderRerelease"));
  Commands[RollbackTransponder] = std::shared_ptr<::RollbackTransponder>(
      new ::RollbackTransponder("RollbackTransponder"));

  Commands[PrintBoxSticker] = std::shared_ptr<::AbstractClientCommand>(
      new ::PrintBoxSticker("PrintBoxSticker"));
  Commands[PrintLastBoxSticker] = std::shared_ptr<::AbstractClientCommand>(
      new ::PrintLastBoxSticker("PrintLastBoxSticker"));
  Commands[PrintPalletSticker] = std::shared_ptr<::AbstractClientCommand>(
      new ::PrintPalletSticker("PrintPalletSticker"));
  Commands[PrintLastPalletSticker] = std::shared_ptr<::AbstractClientCommand>(
      new ::PrintLastPalletSticker("PrintLastPalletSticker"));
}

void PersoServerConnection::clearBuffer() {
  ReceivedDataBlock.clear();
  ReceivedDataBlockSize = 0;
}

void PersoServerConnection::socketConnected_slot() {
  sendLog("Соединение с сервером персонализации установлено. ");
}

void PersoServerConnection::socketDisconnected_slot() {
  sendLog("Соединение с сервером персонализации отключено. ");

  // Если отключение произошло в результате ошибки
  if (Socket->error() != QTcpSocket::UnknownSocketError) {
    emit disconnected();
  }
}

void PersoServerConnection::socketReadyRead_slot() {
  QDataStream deserializator(Socket.get());  // Дессериализатор
  deserializator.setVersion(
      QDataStream::Qt_6_4);  // Настраиваем версию десериализатора

  // Если блок данных еще не начал формироваться
  if (ReceivedDataBlockSize == 0) {
    // Если размер поступивших байт меньше размера поля с размером байт, то
    // блок поступившие данные отбрасываются
    if (Socket->bytesAvailable() < static_cast<int64_t>(sizeof(uint32_t))) {
      sendLog(
          "Размер полученных данных слишком мал. Ожидается прием следующих "
          "частей. ");
      // Перезапускаем таймер ожидания для следующих частей
      WaitTimer->start();
      return;
    }
    // Сохраняем размер блока данных
    deserializator >> ReceivedDataBlockSize;

    sendLog(QString("Размер ожидаемого блока данных: %1.")
                .arg(QString::number(ReceivedDataBlockSize)));

    // Если размер блока данных слишком большой, то весь блок отбрасывается
    if (ReceivedDataBlockSize > DATA_BLOCK_MAX_SIZE) {
      sendLog("Размер блока данных слишком большой. Сброс. ");
      // Останавливаем цикл ожидания
      emit stopResponseWaiting();
      ReceivedDataBlockSize = 0;
      return;
    }
  }

  // Дожидаемся пока весь блок данных придет целиком
  if (Socket->bytesAvailable() < ReceivedDataBlockSize) {
    sendLog("Блок получен не целиком. Ожидается прием следующих частей. ");
    // Перезапускаем таймер ожидания для следующих частей
    WaitTimer->start();
    return;
  }

  // Если блок был получен целиком, то осуществляем его дессериализацию
  deserializator >> ReceivedDataBlock;

  sendLog(QString("Размер полученного блока данных: %1. ")
              .arg(QString::number(ReceivedDataBlock.size())));
  // Ограничиваем вывод в лог полученного блока данных, чтобы не было фризов при
  // отрисовке
  sendLog(QString("Полученный блок данных: %1 ").arg(ReceivedDataBlock));

  // Останавливаем цикл ожидания
  emit stopResponseWaiting();
}

void PersoServerConnection::socketError_slot(
    QAbstractSocket::SocketError socketError) {
  Socket->abort();
  sendLog(QString("Ошибка сети: %1. %2.")
              .arg(QString::number(socketError), Socket->errorString()));
}

void PersoServerConnection::waitTimerTimeout_slot() {
  //    Socket->abort();
  sendLog("Время ожидания вышло. ");
}
