#include "postgre_sql_database.h"

PostgreSqlDatabase::PostgreSqlDatabase(QObject* parent,
                                       const QString& connectionName)
    : AbstractSqlDatabase{parent} {
  setObjectName("PostgreSqlDatabase");
  loadSettings();

  ConnectionName = connectionName;
}

PostgreSqlDatabase::~PostgreSqlDatabase() {
  // Удаляем соединение
  QSqlDatabase::removeDatabase(ConnectionName);
}

void PostgreSqlDatabase::applySettings() {
  sendLog("Применение новых настроек. ");
  loadSettings();

  if (QSqlDatabase::database(ConnectionName).isValid()) {
    sendLog("Удаление предыущего подключения к базе данных. ");
    QSqlDatabase::removeDatabase(ConnectionName);

    sendLog("Создание нового подключения к базе данных. ");
    createDatabaseConnection();
  }
}

bool PostgreSqlDatabase::connect() {
  if (QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres уже установлено. ");
    return true;
  }

  // Создаем соединение
  createDatabaseConnection();
  if (!QSqlDatabase::database(ConnectionName).open()) {
    sendLog(
        QString("Соединение с Postgres не может быть установлено. Ошибка: %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }
  sendLog("Соединение с Postgres установлено. ");

  if (!init()) {
    sendLog("Получена ошибка при инициализации мета-данных. ");
    return false;
  }
  sendLog("Инициализация мета-данных успешно завершена. ");

  return true;
}

void PostgreSqlDatabase::disconnect() {
  // Удаляем соединение
  QSqlDatabase::removeDatabase(ConnectionName);

  if (QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не отключено. ");
  } else {
    sendLog("Соединение с Postgres отключено. ");
  }
}

bool PostgreSqlDatabase::isConnected() {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    return false;
  }

  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (request.exec(";")) {
    return true;
  } else {
    sendLog(request.lastError().text());
    return false;
  }
}

bool PostgreSqlDatabase::openTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  return QSqlDatabase::database(ConnectionName).transaction();
}

bool PostgreSqlDatabase::commitTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  return QSqlDatabase::database(ConnectionName).commit();
}

bool PostgreSqlDatabase::rollbackTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  return QSqlDatabase::database(ConnectionName).rollback();
}

Qt::SortOrder PostgreSqlDatabase::getCurrentOrder() const {
  return CurrentOrder;
}

void PostgreSqlDatabase::setCurrentOrder(Qt::SortOrder order) {
  CurrentOrder = order;
}

uint32_t PostgreSqlDatabase::getRecordMaxCount() const {
  return RecordMaxCount;
}

void PostgreSqlDatabase::setRecordMaxCount(uint32_t count) {
  RecordMaxCount = count;
}

bool PostgreSqlDatabase::execCustomRequest(
    const QString& requestText,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    // Обработка ошибки выполнения запроса
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  // Обработка полученного ответа
  sendLog("Запрос выполнен. ");
  extractRecords(request, records);
  return true;
}

bool PostgreSqlDatabase::createRecords(
    const QString& table,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  // Проверка подключения
  //  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
  //    sendLog(
  //        QString("Соединение с Postgres не установлено. Ошибка: %1.")
  //            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
  //    return false;
  //  }

  //  // Создаем запрос
  //  QString requestText = QString("INSERT INTO public.%1 (").arg(table);
  //  for (QHash<QString, QString>::const_iterator it = record.constBegin();
  //       it != record.constEnd(); it++) {
  //    requestText += QString("%1, ").arg(it.key());
  //  }
  //  requestText.chop(2);
  //  requestText += ") VALUES (";
  //  for (QHash<QString, QString>::const_iterator it = record.constBegin();
  //       it != record.constEnd(); it++) {
  //    if (it.value() != "NULL") {
  //      requestText += QString("'%1', ").arg(it.value());
  //    } else {
  //      requestText += QString("%1, ").arg(it.value());
  //    }
  //  }
  //  requestText.chop(2);
  //  requestText += ");";

  return true;
}

bool PostgreSqlDatabase::readRecords(
    const QString& table,
    const QHash<QString, QString>& searchValues,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  return true;
}

bool PostgreSqlDatabase::readMergedRecords(
    const QStringList& tables,
    const QHash<QString, QString>& searchValues,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  return true;
}

bool PostgreSqlDatabase::updateRecords(
    const QString& table,
    const QString& condition,
    QHash<QString, QString>& newValues) const {
  return true;
}

bool PostgreSqlDatabase::deleteRecords(const QString& table,
                                       const QString& condition) const {
  return true;
}

bool PostgreSqlDatabase::clearTable(const QString& table) const {
  return true;
}

void PostgreSqlDatabase::sendLog(const QString& log) const {
  if (LogEnable) {
    emit const_cast<PostgreSqlDatabase*>(this)->logging(
        QString("%1 - %2").arg(objectName(), log));
  }
}

void PostgreSqlDatabase::loadSettings() {
  // Загружаем настройки
  QSettings settings;

  LogEnable = settings.value("postgre_sql_database/log_enable").toBool();

  HostAddress =
      QHostAddress(settings.value("postgre_sql_database/server_ip").toString());
  HostPort = settings.value("postgre_sql_database/server_port").toInt();
  DatabaseName =
      settings.value("postgre_sql_database/database_name").toString();
  UserName = settings.value("postgre_sql_database/user_name").toString();
  UserPassword =
      settings.value("postgre_sql_database/user_password").toString();
}

void PostgreSqlDatabase::createDatabaseConnection() {
  QSqlDatabase postgres = QSqlDatabase::addDatabase("QPSQL", ConnectionName);

  postgres.setHostName(HostAddress.toString());
  postgres.setPort(HostPort);
  postgres.setDatabaseName(DatabaseName);
  postgres.setUserName(UserName);
  postgres.setPassword(UserPassword);
}

bool PostgreSqlDatabase::init() {
  QSqlDatabase db = QSqlDatabase::database(ConnectionName);
  QStringList tableNames = QSqlDatabase::database(ConnectionName).tables();

  for (int32_t i = 0; i < tableNames.size(); i++) {
    QSharedPointer<PostgreSqlTable> table(
        new PostgreSqlTable(tableNames.at(i), ConnectionName));
    if (!table->init()) {
      sendLog(QString("Получена ошибка при инициализации таблицы '%1'")
                  .arg(tableNames.at(i)));
      return false;
    }

    Tables.insert(tableNames.at(i), table);
  }

  return true;
}

bool PostgreSqlDatabase::checkTableName(const QString& name) {
  return true;
}

bool PostgreSqlDatabase::checkTableField(const QString& field) {
  return true;
}

bool PostgreSqlDatabase::getTableComlumnNames(const QString& name) {
  return true;
}

void PostgreSqlDatabase::extractRecords(
    QSqlQuery& request,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  QSharedPointer<QHash<QString, QString>> record;
  records.clear();
  records.resize(request.size());

  while (request.next()) {
    QSharedPointer<QHash<QString, QString>> record(
        new QHash<QString, QString>());

    for (int32_t j = 0; j < request.record().count(); j++) {
      record->insert(request.record().fieldName(j),
                     request.value(j).toString());
    }

    records.append(record);
  }
}
