#include "postgre_sql_database.h"
#include "definitions.h"

PostgreSqlDatabase::PostgreSqlDatabase(const QString& name)
    : AbstractSqlDatabase{name} {
  ConnectionName = QString("%1%2").arg(name, "Connection");

  doLoadSettings();
}

PostgreSqlDatabase::~PostgreSqlDatabase() {
  // Удаляем соединение
  QSqlDatabase::removeDatabase(ConnectionName);
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

  Tables.clear();
}

bool PostgreSqlDatabase::isConnected() {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    return false;
  }

  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec("")) {
    sendLog(request.lastError().text());
    sendLog("Получена ошибка при выполнении тестового запроса.");
    return false;
  }

  return true;
}

bool PostgreSqlDatabase::openTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec("BEGIN;")) {
    sendLog(request.lastError().text());
    sendLog("Получена ошибка при открытии транзакции.");
    return false;
  }

  return true;
}

bool PostgreSqlDatabase::commitTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec("COMMIT;")) {
    sendLog(request.lastError().text());
    sendLog("Получена ошибка при закрытии транзакции");
    return false;
  }

  return true;
}

bool PostgreSqlDatabase::rollbackTransaction() const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec("ROLLBACK;")) {
    sendLog(request.lastError().text());
    sendLog("Получена ошибка при откате транзакции");
    return false;
  }

  return true;
}

Qt::SortOrder PostgreSqlDatabase::getCurrentOrder() const {
  if (CurrentOrder == "ASC") {
    return Qt::AscendingOrder;
  }

  return Qt::DescendingOrder;
}

void PostgreSqlDatabase::setCurrentOrder(Qt::SortOrder order) {
  if (order == Qt::AscendingOrder) {
    CurrentOrder = "ASC";
  } else {
    CurrentOrder = "DESC";
  }

  for (auto it1 = Tables.cbegin(), it2 = Tables.cend(); it1 != it2; ++it1) {
    it1->second->setCurrentOrder(order);
  }
}

uint32_t PostgreSqlDatabase::getRecordMaxCount() const {
  return RecordMaxCount;
}

void PostgreSqlDatabase::setRecordMaxCount(uint32_t count) {
  RecordMaxCount = count;

  for (auto it = Tables.cbegin(); it != Tables.cend(); it++) {
    it->second->setRecordMaxCount(count);
  }
}

bool PostgreSqlDatabase::execCustomRequest(const QString& requestText,
                                           SqlQueryValues& response) const {
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
  response.extractRecords(request);
  return true;
}

bool PostgreSqlDatabase::createRecords(const QString& table,
                                       const SqlQueryValues& records) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->createRecords(records);
}

bool PostgreSqlDatabase::readRecords(const QString& table,
                                     SqlQueryValues& response) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->readRecords(response);
}

bool PostgreSqlDatabase::readRecords(const QString& table,
                                     const QString& conditions,
                                     SqlQueryValues& response) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->readRecords(conditions, response);
}

bool PostgreSqlDatabase::readLastRecord(const QString& table,
                                        SqlQueryValues& record) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->readLastRecord(record);
}

bool PostgreSqlDatabase::updateRecords(const QString& table,
                                       const SqlQueryValues& newValues) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->updateRecords(newValues);
}

bool PostgreSqlDatabase::updateRecords(const QString& table,
                                       const QString& conditions,
                                       const SqlQueryValues& newValues) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->updateRecords(conditions, newValues);
}

bool PostgreSqlDatabase::deleteRecords(const QString& table,
                                       const QString& condition) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (Tables.count(table) == 0) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  return Tables.at(table)->deleteRecords(condition);
}

bool PostgreSqlDatabase::clearTable(const QString& table) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  return Tables.at(table)->clear();
}

bool PostgreSqlDatabase::readMergedRecords(const QStringList& tables,
                                           const QString& conditions,
                                           SqlQueryValues& response) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (!checkTableNames(tables)) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  // Создаем запрос
  QString requestText;
  if (response.fieldCount() == 0) {
    requestText += QString("SELECT %1.* FROM public.%1 ").arg(tables.first());
  } else {
    requestText += "SELECT ";
    for (int32_t i = 0; i < response.fieldCount(); i++) {
      requestText += response.fieldName(i) + ", ";
    }
    requestText.chop(2);

    requestText += QString(" FROM public.%1 ").arg(tables.first());
  }

  for (int32_t i = 1; i < tables.size(); i++) {
    requestText +=
        QString("JOIN %1 ON %2.%3 = %1.%4 ")
            .arg(tables.at(i), tables.at(i - 1),
                 Tables.at(tables.at(i - 1))->relations()->value(tables.at(i)),
                 Tables.at(tables.at(i))->getPrimaryKey());
  }

  requestText +=
      QString("WHERE %1 ORDER BY %2.%3 %4 ")
          .arg(conditions, tables.first(),
               Tables.at(tables.first())->getPrimaryKey(), CurrentOrder);
  if (RecordMaxCount > 0) {
    requestText += QString("LIMIT %1").arg(QString::number(RecordMaxCount));
  }
  requestText += ";";

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  response.extractRecords(request);
  return true;
}

bool PostgreSqlDatabase::updateMergedRecords(
    const QStringList& tables,
    const QString& conditions,
    const SqlQueryValues& newValues) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (!checkTableNames(tables)) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  //  UPDATE transponders
  //  SET awaiting_confirmation = true
  //  WHERE transponders.id IN (
  //      SELECT transponders.id FROM transponders
  //      JOIN boxes ON transponders.box_id = boxes.id
  //      JOIN pallets ON boxes.pallet_id = pallets.id
  //      WHERE pallets.id = 1000001);

  // Создаем запрос
  QString requestText = QString("UPDATE public.%1 SET ").arg(tables.first());
  for (int32_t i = 0; i < newValues.fieldCount(); i++) {
    if (newValues.get(i) == "NULL") {
      requestText += QString("%1 = NULL, ").arg(newValues.fieldName(i));
    } else {
      requestText +=
          QString("%1 = '%2', ").arg(newValues.fieldName(i), newValues.get(i));
    }
  }

  requestText.chop(2);
  requestText +=
      QString(" WHERE %1.%2 IN (SELECT %1.%2 FROM public.%1 ")
          .arg(tables.first(), Tables.at(tables.first())->getPrimaryKey());
  for (int32_t i = 1; i < tables.size(); i++) {
    requestText +=
        QString("JOIN %1 ON %2.%3 = %1.%4 ")
            .arg(tables.at(i), tables.at(i - 1),
                 Tables.at(tables.at(i - 1))->relations()->value(tables.at(i)),
                 Tables.at(tables.at(i))->getPrimaryKey());
  }
  requestText += QString(" WHERE %1);").arg(conditions);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlDatabase::deleteMergedRecords(const QStringList& tables,
                                             const QString& conditions) const {
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog("Соединение с Postgres не установлено. ");
    return false;
  }

  if (!checkTableNames(tables)) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  QString requestText =
      QString(
          "DELETE FROM public.%1 WHERE %1.%2 IN (SELECT %1.%2 FROM public.%1 ")
          .arg(tables.first(), Tables.at(tables.first())->getPrimaryKey());
  for (int32_t i = 1; i < tables.size(); i++) {
    requestText +=
        QString("JOIN %1 ON %2.%3 = %1.%4 ")
            .arg(tables.at(i), tables.at(i - 1),
                 Tables.at(tables.at(i - 1))->relations()->value(tables.at(i)),
                 Tables.at(tables.at(i))->getPrimaryKey());
  }
  requestText += QString(" WHERE %1);").arg(conditions);

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));

  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  return true;
}

bool PostgreSqlDatabase::getRecordCount(const QString& table,
                                        uint32_t& count) const {
  return Tables.at(table)->getRecordCount(count);
}

bool PostgreSqlDatabase::getLastId(const QString& table, int32_t& id) const {
  SqlQueryValues record;
  id = 0;

  if (!readLastRecord(table, record)) {
    sendLog(
        QString("Получена ошибка при поиске последней записи в таблице '%1'. ")
            .arg(table));
    return false;
  }
  if (record.isEmpty()) {
    if (table == "transponders") {
      id = TRANSPONDER_ID_START_SHIFT;
    }
    if (table == "boxes") {
      id = BOX_ID_START_SHIFT;
    }
    if (table == "pallets") {
      id = PALLET_ID_START_SHIFT;
    }
    return true;
  }

  id = record.get("id").toInt();
  return true;
}

/*
 *   Приватные методы
 */

void PostgreSqlDatabase::loadSettings() {
  doLoadSettings();

  if (QSqlDatabase::database(ConnectionName).isValid()) {
    sendLog("Удаление предыущего подключения к базе данных. ");
    QSqlDatabase::removeDatabase(ConnectionName);

    sendLog("Создание нового подключения к базе данных. ");
    createDatabaseConnection();
  }
}

void PostgreSqlDatabase::doLoadSettings() {
  // Загружаем настройки
  QSettings settings;

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
  QStringList tableNames = QSqlDatabase::database(ConnectionName).tables();

  for (int32_t i = 0; i < tableNames.size(); i++) {
    if (!createTable(tableNames[i])) {
      return false;
    }
  }

  return true;
}

bool PostgreSqlDatabase::createTable(const QString& name) {
  std::unique_ptr<PostgreSqlTable> table(new PostgreSqlTable(
      QString("PostgreSQL table '%1'").arg(name), name, ConnectionName));
  if (!table->init()) {
    sendLog(
        QString("Получена ошибка при инициализации таблицы '%1'").arg(name));
    return false;
  }

  Tables[name] = std::move(table);

  return true;
}

bool PostgreSqlDatabase::checkTableNames(const QStringList& names) const {
  for (auto it1 = names.cbegin(), it2 = names.cend(); it1 != it2; ++it1) {
    if (Tables.count(*it1) == 0) {
      return false;
    }
  }

  return true;
}
