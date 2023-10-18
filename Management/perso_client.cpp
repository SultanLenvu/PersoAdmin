#include "perso_client.h"

PersoClient::PersoClient(QObject *parent) : QObject(parent)
{
  setObjectName("PersoClient");
  loadSettings();
  TimeoutIndicator = false;

  // Создаем сокет
  createSocket();

  // Создаем таймеры
  createTimers();

  // Готовы к выполнению команд
  CurrentState = Ready;
}

PersoClient::~PersoClient() {
  if (Socket->isOpen())
    Socket->disconnectFromHost();
}

PersoClient::ReturnStatus PersoClient::connectToServer() {
  // Подключаемся к серверу персонализации
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  return Completed;
}

PersoClient::ReturnStatus PersoClient::disconnectFromServer() {
  if (Socket->isOpen()) {
    Socket->disconnectFromHost();
    sendLog("Отключение от сервера персонализации. ");
  } else {
    sendLog("Подключение не было установлено. ");
  }

  return Completed;
}

PersoClient::ReturnStatus PersoClient::requestEcho() {
  // Создаем запрос
  CurrentState = CreatingRequest;
  createEcho();

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processEcho();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestAuthorize(
    const QHash<QString, QString>* requestAttributes) {
  // Создаем запрос
  CurrentState = CreatingRequest;

  // Проверка на существование
  if (!requestAttributes) {
    sendLog("Получены не корректные параметры запроса. Сброс.");
    return RequestParameterError;
  }
  createAuthorization(requestAttributes);

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processAuthorization();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestTransponderRelease(
    const QHash<QString, QString>* requestAttributes,
    QFile* firmware) {
  // Создаем запрос
  CurrentState = CreatingRequest;

  // Проверка на существование
  if ((!requestAttributes) || (!firmware)) {
    sendLog("Получены не корректные параметры запроса. Сброс.");
    return RequestParameterError;
  }
  createTransponderRelease(requestAttributes);

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processTransponderRelease(firmware);

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestTransponderReleaseConfirm(
    const QHash<QString, QString>* requestAttributes,
    QHash<QString, QString>* responseAttributes) {
  // Создаем запрос
  CurrentState = CreatingRequest;

  // Проверка на существование
  if ((!requestAttributes) || (!responseAttributes)) {
    sendLog("Получены не корректные параметры запроса. Сброс.");
    return RequestParameterError;
  }
  createTransponderReleaseConfirm(requestAttributes);

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processTransponderReleaseConfirm(responseAttributes);

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestTransponderRerelease(
    const QHash<QString, QString>* requestAttributes,
    QFile* firmware) {
  // Создаем запрос
  CurrentState = CreatingRequest;

  // Проверка на существование
  if ((!requestAttributes) || (!firmware)) {
    sendLog("Получены не корректные параметры запроса. Сброс.");
    return RequestParameterError;
  }
  createTransponderRerelease(requestAttributes);

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processTransponderRerelease(firmware);

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestTransponderRereleaseConfirm(
    const QHash<QString, QString>* requestAttributes,
    QHash<QString, QString>* responseAttributes) {
  // Создаем запрос
  CurrentState = CreatingRequest;

  // Проверка на существование
  if ((!requestAttributes) || (!responseAttributes)) {
    sendLog("Получены не корректные параметры запроса. Сброс.");
    return RequestParameterError;
  }
  createTransponderRereleaseConfirm(requestAttributes);

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processTransponderRereleaseConfirm(responseAttributes);

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestBoxStickerPrint() {
  // Создаем запрос
  CurrentState = CreatingRequest;

  createBoxStickerPrint();

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processBoxStickerPrint();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestBoxStickerReprint() {
  // Создаем запрос
  CurrentState = CreatingRequest;

  createBoxStickerReprint();

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processBoxStickerReprint();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestPalletStickerPrint() {
  // Создаем запрос
  CurrentState = CreatingRequest;

  createPalletStickerPrint();

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processPalletStickerPrint();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

PersoClient::ReturnStatus PersoClient::requestPalletStickerReprint() {
  // Создаем запрос
  CurrentState = CreatingRequest;

  processPalletStickerReprint();

  // Подключаемся к серверу персонализации
  CurrentState = WaitingServerConnection;
  if (!processingServerConnection()) {
    return ServerConnectionError;
  }

  // Отправляем сформированный блок данных
  CurrentState = WaitingResponse;
  transmitDataBlock();

  // Если сервер не отвечает
  if (TimeoutIndicator) {
    return ServerNotResponding;
  }

  // Отключаемся от сервера
  Socket->close();

  // Обрабатываем полученный ответ
  CurrentState = ProcessingResponse;
  processPalletStickerReprint();

  // Очищаем команду и ответ на нее
  CurrentCommand = QJsonObject();
  CurrentResponse = QJsonObject();

  // Замыкаем машину состояний
  CurrentState = Ready;

  // Возвращаем статус выполнения команды
  return ProcessingStatus;
}

void PersoClient::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();
}


/*
 * Приватные  методы
 */

void PersoClient::loadSettings() {
  QSettings settings;

  LogEnable = settings.value("log_system/global_enable").toBool();
  ExtendedLoggingEnable = settings.value("log_system/extended_enable").toBool();

  PersoServerAddress = settings.value("perso_client/server_ip").toString();
  PersoServerPort = settings.value("perso_client/server_port").toInt();
}

void PersoClient::sendLog(const QString& log) {
  if (LogEnable) {
    emit logging(QString("%1 - %2").arg(objectName(), log));
  }
}

void PersoClient::createTimers() {
  WaitTimer = new QTimer(this);
  WaitTimer->setInterval(PERSO_SERVER_CONNECTION_WAITING_TIME);
  connect(WaitTimer, &QTimer::timeout, this,
          &PersoClient::on_WaitTimerTimeout_slot);
  connect(WaitTimer, &QTimer::timeout, WaitTimer, &QTimer::stop);
  connect(Socket, &QTcpSocket::readyRead, WaitTimer, &QTimer::stop);
  connect(Socket, &QTcpSocket::connected, WaitTimer, &QTimer::stop);
  connect(Socket, &QTcpSocket::disconnected, WaitTimer, &QTimer::stop);
  connect(Socket,
          QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
          WaitTimer, &QTimer::stop);

  WaitingLoop = new QEventLoop(this);
  connect(Socket, &QTcpSocket::connected, WaitingLoop, &QEventLoop::quit);
  connect(Socket, &QTcpSocket::disconnected, WaitingLoop, &QEventLoop::quit);
  connect(Socket,
          QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
          WaitingLoop, &QEventLoop::quit);
  connect(WaitTimer, &QTimer::timeout, WaitingLoop, &QEventLoop::quit);
  connect(this, &PersoClient::stopResponseWaiting, WaitingLoop,
          &QEventLoop::quit);
}

void PersoClient::createSocket() {
  Socket = new QTcpSocket(this);
  connect(Socket, &QTcpSocket::connected, this,
          &PersoClient::on_SocketConnected_slot);
  connect(Socket, &QTcpSocket::disconnected, this,
          &PersoClient::on_SocketDisconnected_slot);
  connect(Socket, &QTcpSocket::readyRead, this,
          &PersoClient::on_SocketReadyRead_slot);
  connect(Socket,
          QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this,
          &PersoClient::on_SocketError_slot);
}

bool PersoClient::processingServerConnection() {
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
    return false;
  }

  return true;
}

void PersoClient::processingDataBlock() {
  QJsonParseError status;
  QJsonDocument responseDocument =
      QJsonDocument::fromJson(ReceivedDataBlock, &status);

  // Если пришел некорректный JSON
  if (status.error != QJsonParseError::NoError) {
    sendLog("Ошибка парсинга JSON команды. ");
    return;
  }

  sendLog(QString("Размер ответа на команду: %1.")
              .arg(QString::number(responseDocument.toJson().size())));
  if (ExtendedLoggingEnable == true) {
    sendLog(QString("Содержание ответа: %1 ")
                .arg(QString(responseDocument.toJson())));
  }

  // Выделяем список пар ключ-значение из JSON-файла
  CurrentResponse = responseDocument.object();
}

void PersoClient::createTransmittedDataBlock(void) {
  QJsonDocument requestDocument(CurrentCommand);

  sendLog(QString("Размер команды: %1 ")
              .arg(QString::number(requestDocument.toJson().size())));
  if (ExtendedLoggingEnable == true) {
    sendLog(QString("Содержание команды: %1 ")
                .arg(QString(requestDocument.toJson())));
  }
  sendLog("Формирование блока данных для команды. ");

  // Инициализируем блок данных и сериализатор
  TransmittedDataBlock.clear();
  QDataStream serializator(&TransmittedDataBlock, QIODevice::WriteOnly);
  serializator.setVersion(QDataStream::Qt_5_12);

  // Формируем единый блок данных для отправки
  serializator << uint32_t(0) << requestDocument.toJson();
  serializator.device()->seek(0);
  serializator << uint32_t(TransmittedDataBlock.size() - sizeof(uint32_t));
}

void PersoClient::transmitDataBlock() {
  // Ответный блок данных еще не получен
  ReceivedDataBlockSize = 0;

  // Если размер блок не превышает максимального размера данных для единоразовой
  // передачи
  if (TransmittedDataBlock.size() < ONETIME_TRANSMIT_DATA_SIZE) {
    // Отправляем блок данных
    Socket->write(TransmittedDataBlock);
  } else {
    // В противном случае дробим блок данных на части и последовательно
    // отправляем
    for (int32_t i = 0; i < TransmittedDataBlock.size();
         i += ONETIME_TRANSMIT_DATA_SIZE) {
      Socket->write(TransmittedDataBlock.mid(i, ONETIME_TRANSMIT_DATA_SIZE));
    }
  }

  // Ожидаем ответ
  WaitTimer->start();
  WaitingLoop->exec();
}

void PersoClient::createEcho(void) {
  sendLog("Формирование команды echo. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "echo";

  // Тело команды
  CurrentCommand["data"] = "Test";

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createAuthorization(
    const QHash<QString, QString>* requestAttributes) {
  sendLog("Формирование команды athorization. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "authorization";

  // Тело команды
  CurrentCommand["login"] = requestAttributes->value("login");
  CurrentCommand["password"] = requestAttributes->value("password");

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createTransponderRelease(
    const QHash<QString, QString>* requestAttributes) {
  sendLog("Формирование команды transponder_release. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "transponder_release";

  // Тело команды
  CurrentCommand["login"] = requestAttributes->value("login");
  CurrentCommand["password"] = requestAttributes->value("password");

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createTransponderReleaseConfirm(
    const QHash<QString, QString>* requestAttributes) {
  sendLog("Формирование команды transponder_release_confirm. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "transponder_release_confirm";

  // Тело команды
  CurrentCommand["login"] = requestAttributes->value("login");
  CurrentCommand["password"] = requestAttributes->value("password");
  CurrentCommand["ucid"] = requestAttributes->value("ucid");

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createTransponderRerelease(
    const QHash<QString, QString>* requestAttributes) {
  sendLog("Формирование команды transponder_rerelease. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "transponder_rerelease";

  // Тело команды
  CurrentCommand["login"] = requestAttributes->value("login");
  CurrentCommand["password"] = requestAttributes->value("password");
  CurrentCommand["pan"] = requestAttributes->value("pan");

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createTransponderRereleaseConfirm(
    const QHash<QString, QString>* requestAttributes) {
  sendLog("Формирование команды transponder_rerelease_confirm. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "transponder_rerelease_confirm";

  // Тело команды
  CurrentCommand["login"] = requestAttributes->value("login");
  CurrentCommand["password"] = requestAttributes->value("password");
  CurrentCommand["pan"] = requestAttributes->value("pan");
  CurrentCommand["ucid"] = requestAttributes->value("ucid");

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createBoxStickerPrint() {
  sendLog("Формирование команды print_box_sticker. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "print_box_sticker";

  // Тело команды
  CurrentCommand["id"] = "1234567890";
  CurrentCommand["quantity"] = "50";
  CurrentCommand["first_transponder_sn"] = "5012340000000001";
  CurrentCommand["last_transponder_sn"] = "5012340000000001";
  CurrentCommand["transponder_model"] = "PS1001";

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createBoxStickerReprint() {
  sendLog("Формирование команды print_last_box_sticker. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "print_last_box_sticker";

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createPalletStickerPrint() {
  sendLog("Формирование команды print_pallet_sticker. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "print_pallet_sticker";

  // Тело команды
  CurrentCommand["id"] = "1234567890";
  CurrentCommand["quantity"] = "50";
  CurrentCommand["first_box_id"] = "5012340000000001";
  CurrentCommand["last_box_id"] = "5012340000000001";
  CurrentCommand["transponder_model"] = "PS1001";
  CurrentCommand["assembly_date"] = "18.10.2023";

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::createPalletStickerReprint() {
  sendLog("Формирование команды print_last_pallet_sticker. ");

  // Заголовок команды
  CurrentCommand["command_name"] = "print_last_pallet_sticker";

  // Создаем блок данных для команды
  createTransmittedDataBlock();
}

void PersoClient::processEcho(void) {
  sendLog("Обработка ответа на команду Echo. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  if (CurrentResponse.value("data").isUndefined() ||
      CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() != "echo")) {
    sendLog("Обнаружена синтаксическая ошибка в ответе Echo. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  sendLog("Команда Echo успешно выполнена. ");
  ProcessingStatus = Completed;
}

void PersoClient::processAuthorization(void) {
  sendLog("Обработка ответа на команду TransponderRelease. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() != "authorization") ||
      CurrentResponse.value("access").isUndefined()) {
    sendLog("Обнаружена синтаксическая ошибка в ответе Authorization. Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  if (CurrentResponse.value("access").toString() == "allowed") {
    ProcessingStatus = Completed;
  } else if (CurrentResponse.value("access").toString() == "not_exist") {
    ProcessingStatus = AuthorizationNotExist;
  } else if (CurrentResponse.value("access").toString() == "not_active") {
    ProcessingStatus = AuthorizationNotActive;
  } else {
    ProcessingStatus = AuthorizationAccessDenied;
  }

  sendLog("Команда Authorization успешно выполнена. ");
}

void PersoClient::processTransponderRelease(QFile* firmware) {
  sendLog("Обработка ответа на команду transponder_release. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "transponder_release") ||
      CurrentResponse.value("firmware").isUndefined()) {
    sendLog("Обнаружена синтаксическая ошибка в ответе TransponderRelease. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Сохраняем присланный файл прошивки
  if (!firmware->open(QIODevice::WriteOnly)) {
    sendLog("Не удалось сохранить файл прошивки. ");
    ProcessingStatus = ServerError;
    return;
  }

  // Сохраняем прошивку в файл
  firmware->write(QByteArray::fromBase64(
      CurrentResponse.value("firmware").toString().toUtf8()));
  firmware->close();

  ProcessingStatus = Completed;
  sendLog("Команда TransponderRelease успешно выполнена. ");
}

void PersoClient::processTransponderReleaseConfirm(
    QHash<QString, QString>* responseAttributes) {
  sendLog("Обработка ответа на команду transponder_release_confirm. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "transponder_release_confirm") ||
      CurrentResponse.value("sn").isUndefined() ||
      CurrentResponse.value("pan").isUndefined() ||
      CurrentResponse.value("box_id").isUndefined() ||
      CurrentResponse.value("pallet_id").isUndefined() ||
      CurrentResponse.value("order_id").isUndefined() ||
      CurrentResponse.value("issuer_name").isUndefined() ||
      CurrentResponse.value("transponder_model").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе "
        "transponder_release_confirm. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  responseAttributes->insert("sn", CurrentResponse.value("sn").toString());
  responseAttributes->insert("pan", CurrentResponse.value("pan").toString());
  responseAttributes->insert("box_id",
                             CurrentResponse.value("box_id").toString());
  responseAttributes->insert("pallet_id",
                             CurrentResponse.value("pallet_id").toString());
  responseAttributes->insert("order_id",
                             CurrentResponse.value("order_id").toString());
  responseAttributes->insert("issuer_name",
                             CurrentResponse.value("issuer_name").toString());
  responseAttributes->insert(
      "transponder_model",
      CurrentResponse.value("transponder_model").toString());

  ProcessingStatus = Completed;
  sendLog("Команда TransponderReleaseConfirm успешно выполнена. ");
}

void PersoClient::processTransponderRerelease(QFile* firmware) {
  sendLog("Обработка ответа на команду transponder_rerelease. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "transponder_rerelease") ||
      CurrentResponse.value("firmware").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе TransponderRerelease: "
        "отсутствует файл прошивки. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Сохраняем присланный файл прошивки
  if (!firmware->open(QIODevice::WriteOnly)) {
    sendLog("Не удалось сохранить файл прошивки. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Сохраняем прошивку в файл
  firmware->write(QByteArray::fromBase64(
      CurrentResponse.value("firmware").toString().toUtf8()));
  firmware->close();

  ProcessingStatus = Completed;
  sendLog("Команда transponder_rerelease успешно выполнена. ");
}

void PersoClient::processTransponderRereleaseConfirm(
    QHash<QString, QString>* responseAttributes) {
  sendLog("Обработка ответа на команду transponder_rerelease_confirm. ");

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "transponder_rerelease_confirm") ||
      CurrentResponse.value("sn").isUndefined() ||
      CurrentResponse.value("pan").isUndefined() ||
      CurrentResponse.value("box_id").isUndefined() ||
      CurrentResponse.value("pallet_id").isUndefined() ||
      CurrentResponse.value("order_id").isUndefined() ||
      CurrentResponse.value("issuer_name").isUndefined() ||
      CurrentResponse.value("transponder_model").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе TransponderReleaseConfirm. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  responseAttributes->insert("sn", CurrentResponse.value("sn").toString());
  responseAttributes->insert("pan", CurrentResponse.value("pan").toString());
  responseAttributes->insert("box_id",
                             CurrentResponse.value("box_id").toString());
  responseAttributes->insert("pallet_id",
                             CurrentResponse.value("pallet_id").toString());
  responseAttributes->insert("order_id",
                             CurrentResponse.value("order_id").toString());
  responseAttributes->insert("issuer_name",
                             CurrentResponse.value("issuer_name").toString());
  responseAttributes->insert(
      "transponder_model",
      CurrentResponse.value("transponder_model").toString());

  ProcessingStatus = Completed;
  sendLog("Команда transponder_rerelease_confirm успешно выполнена. ");
}

void PersoClient::processBoxStickerPrint() {
  sendLog("Обработка ответа на команду print_box_sticker. ");

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "print_box_sticker") ||
      CurrentResponse.value("return_status").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе print_box_sticker. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  ProcessingStatus = Completed;
  sendLog("Команда print_box_sticker успешно выполнена. ");
}

void PersoClient::processBoxStickerReprint() {
  sendLog("Обработка ответа на команду print_last_box_sticker. ");

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "print_last_box_sticker") ||
      CurrentResponse.value("return_status").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе print_last_box_sticker. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  ProcessingStatus = Completed;
  sendLog("Команда print_last_box_sticker успешно выполнена. ");
}

void PersoClient::processPalletStickerPrint() {
  sendLog("Обработка ответа на команду print_pallet_sticker. ");

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "print_pallet_sticker") ||
      CurrentResponse.value("return_status").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе print_pallet_sticker. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  ProcessingStatus = Completed;
  sendLog("Команда print_pallet_sticker успешно выполнена. ");
}

void PersoClient::processPalletStickerReprint() {
  sendLog("Обработка ответа на команду print_last_pallet_sticker. ");

  // Синтаксическая проверка
  if (CurrentResponse.value("response_name").isUndefined() ||
      (CurrentResponse.value("response_name").toString() !=
       "print_last_pallet_sticker") ||
      CurrentResponse.value("return_status").isUndefined()) {
    sendLog(
        "Обнаружена синтаксическая ошибка в ответе print_last_pallet_sticker. "
        "Сброс. ");
    ProcessingStatus = ResponseSyntaxError;
    return;
  }

  // Проверка статуса возврата
  if (CurrentResponse["return_status"] != "no_error") {
    sendLog(QString("Получена серверная ошибка: %1")
                .arg(CurrentResponse["return_status"].toString()));
    ProcessingStatus = ServerError;
    return;
  }

  ProcessingStatus = Completed;
  sendLog("Команда print_last_pallet_sticker успешно выполнена. ");
}

void PersoClient::on_SocketDisconnected_slot() {
  sendLog("Соединение с сервером персонализации отключено. ");
}

void PersoClient::on_SocketConnected_slot() {
  sendLog("Соединение с сервером персонализации установлено. ");
}

void PersoClient::on_SocketReadyRead_slot() {
  QDataStream deserializator(Socket);  // Дессериализатор
  deserializator.setVersion(
      QDataStream::Qt_5_12);  // Настраиваем версию десериализатора

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
    }
  }

  sendLog(QString("Размер принятых данных: %1. ")
              .arg(QString::number(Socket->bytesAvailable())));

  // Дожидаемся пока весь блок данных придет целиком
  if (Socket->bytesAvailable() < ReceivedDataBlockSize) {
    sendLog("Блок получен не целиком. Ожидается прием следующих частей. ");
    // Перезапускаем таймер ожидания для следующих частей
    WaitTimer->start();
    return;
  }

  // Если блок был получен целиком, то осуществляем его дессериализацию
  deserializator >> ReceivedDataBlock;

  // Осуществляем первичную обработку полученных данных
  processingDataBlock();

  // Останавливаем цикл ожидания
  emit stopResponseWaiting();
}

void PersoClient::on_SocketError_slot(
    QAbstractSocket::SocketError socketError) {
  Socket->abort();
  sendLog(QString("Ошибка сети: %1. %2.")
              .arg(QString::number(socketError), Socket->errorString()));
}

void PersoClient::on_WaitTimerTimeout_slot() {
  Socket->abort();
  TimeoutIndicator = true;
  sendLog("Время ожидания вышло. ");
}
