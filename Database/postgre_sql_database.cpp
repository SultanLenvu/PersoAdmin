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

bool PostgreSqlDatabase::checkConnection() {
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

  for (QHash<QString, QSharedPointer<PostgreSqlTable>>::const_iterator it =
           Tables.constBegin();
       it != Tables.constEnd(); it++) {
    it.value()->setCurrentOrder(order);
  }
}

uint32_t PostgreSqlDatabase::getRecordMaxCount() const {
  return RecordMaxCount;
}

void PostgreSqlDatabase::setRecordMaxCount(uint32_t count) {
  RecordMaxCount = count;

  for (QHash<QString, QSharedPointer<PostgreSqlTable>>::const_iterator it =
           Tables.constBegin();
       it != Tables.constEnd(); it++) {
    it.value()->setRecordMaxCount(count);
  }
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
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->createRecords(records);
}

bool PostgreSqlDatabase::readRecords(
    const QString& table,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->readRecords(records);
}

bool PostgreSqlDatabase::readRecords(
    const QString& table,
    const QString& conditions,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->readRecords(conditions, records);
}

bool PostgreSqlDatabase::readLastRecord(const QString& table,
                                        QHash<QString, QString>& record) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->readLastRecord(record);
}

bool PostgreSqlDatabase::updateRecords(
    const QString& table,
    const QString& conditions,
    const QHash<QString, QString>& newValues) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->updateRecords(conditions, newValues);
}

bool PostgreSqlDatabase::deleteRecords(const QString& table,
                                       const QString& condition) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->deleteRecords(condition);
}

bool PostgreSqlDatabase::clearTable(const QString& table) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!Tables.contains(table)) {
    sendLog(QString("Таблица %1 отсутствует в базе.").arg(table));
    return false;
  }

  return Tables.value(table)->clear();
}

bool PostgreSqlDatabase::readMergedRecords(
    const QStringList& tables,
    const QString& conditions,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
    return false;
  }

  if (!checkTableNames(tables)) {
    sendLog("Получено неизвестное имя таблицы.");
    return false;
  }

  // Создаем запрос
  QString requestText = QString("SELECT * FROM public.%1 ").arg(tables.first());
  for (int32_t i = 0; i < tables.size(); i++) {
    requestText += QString("JOIN %1 ON %2.%3 = %1.%4 ")
                       .arg(tables.at(i + 1), tables.at(i),
                            Tables.value(tables.at(i))
                                ->relations()
                                ->value(tables.at(i + 1)),
                            Tables.value(tables.at(i + 1))->getPrimaryKey());
  }

  requestText += QString("WHERE %1 LIMIT %2;")
                     .arg(conditions, QString::number(RecordMaxCount));

  // Выполняем запрос
  QSqlQuery request(QSqlDatabase::database(ConnectionName));
  if (!request.exec(requestText)) {
    sendLog(request.lastError().text());
    sendLog("Отправленный запрос: " + requestText);
    return false;
  }

  extractRecords(request, records);
  return true;
}

bool PostgreSqlDatabase::updateMergedRecords(
    const QStringList& tables,
    const QString& conditions,
    const QHash<QString, QString>& newValues) const {
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
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
  QString requestText = QString("UPDATE * public.%1 SET ").arg(tables.first());
  for (QHash<QString, QString>::const_iterator it = newValues.constBegin();
       it != newValues.constEnd(); it++) {
    if ((it.value() == "NULL") || (it.value().isEmpty())) {
      requestText += QString("%1 = NULL, ").arg(it.key());
    } else {
      requestText += QString("%1 = '%2', ").arg(it.key(), it.value());
    }
  }

  requestText.chop(2);
  requestText +=
      QString(" WHERE %1.%2 IN (SELECT %1.%2 FROM public.%1 ")
          .arg(tables.first(), Tables.value(tables.first())->getPrimaryKey());
  for (int32_t i = 0; i < tables.size(); i++) {
    requestText += QString("JOIN %1 ON %2.%3 = %1.%4 ")
                       .arg(tables.at(i + 1), tables.at(i),
                            Tables.value(tables.at(i))
                                ->relations()
                                ->value(tables.at(i + 1)),
                            Tables.value(tables.at(i + 1))->getPrimaryKey());
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
  // Проверка подключения
  if (!QSqlDatabase::database(ConnectionName).isOpen()) {
    sendLog(
        QString("Соединение с Postgres не установлено. %1.")
            .arg(QSqlDatabase::database(ConnectionName).lastError().text()));
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
  QString requestText =
      QString(
          "DELETE FROM public.%1 WHERE %1.%2 IN (SELECT %1.%2 FROM public.%1 ")
          .arg(tables.first(), Tables.value(tables.first())->getPrimaryKey());
  for (int32_t i = 0; i < tables.size(); i++) {
    requestText += QString("JOIN %1 ON %2.%3 = %1.%4 ")
                       .arg(tables.at(i + 1), tables.at(i),
                            Tables.value(tables.at(i))
                                ->relations()
                                ->value(tables.at(i + 1)),
                            Tables.value(tables.at(i + 1))->getPrimaryKey());
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

/*
 *   Приватные методы
 */

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

bool PostgreSqlDatabase::checkTableNames(const QStringList& names) const {
  for (QStringList::const_iterator it = names.constBegin();
       it != names.constEnd(); it++) {
    if (!Tables.contains(*it)) {
      return false;
    }
  }

  return true;
}

void PostgreSqlDatabase::extractRecords(
    QSqlQuery& request,
    QHash<QString, QSharedPointer<QVector<QString>>>& records) const {
  QSharedPointer<QVector<QString>> record;
  records.clear();

  for (int32_t i = 0; i < request.record().count(); i++) {
    QSharedPointer<QVector<QString>> record(new QVector<QString>());
    record->resize(request.size());
    records.insert(request.record().fieldName(i), record);
  }

  while (request.next()) {
    for (int32_t i = 0; i < request.record().count(); i++) {
      records.value(request.record().fieldName(i))
          ->append(request.value(i).toString());
    }
  }
}
