#include "postre_sql_database.h"

PostreSqlDatabase::PostreSqlDatabase(QObject* parent,
                                     const QString& connectionName)
    : AbstractSqlDatabase{parent} {
  setObjectName("PostreSqlDatabase");
  loadSettings();

  ConnectionName = connectionName;
}

PostreSqlDatabase::~PostreSqlDatabase() {
  // Удаляем соединение
  QSqlDatabase::removeDatabase(ConnectionName);
}

void PostreSqlDatabase::applySettings() {}

bool PostreSqlDatabase::connect() {}

void PostreSqlDatabase::disconnect() {}

bool PostreSqlDatabase::isConnected() {}

bool PostreSqlDatabase::openTransaction() const {}

bool PostreSqlDatabase::closeTransaction() const {}

bool PostreSqlDatabase::abortTransaction() const {}

Qt::SortOrder PostreSqlDatabase::getCurrentOrder() const {}

void PostreSqlDatabase::setCurrentOrder(Qt::SortOrder order) {}

uint32_t PostreSqlDatabase::getRecordMaxCount() const {}

void PostreSqlDatabase::setRecordMaxCount(uint32_t count) const {}

bool PostreSqlDatabase::createRecord(
    const QString& table,
    QVector<QHash<QString, QString>>& records) const {}

bool PostreSqlDatabase::readRecords(
    const QString& table,
    const QHash<QString, QString>& searchValues,
    QVector<QSharedPointer<QHash<QString, QString>>>& records) const {}

bool PostreSqlDatabase::readMergedRecords(
    const QStringList& tables,
    const QHash<QString, QString>& searchValues,
    QVector<QSharedPointer<QHash<QString, QString>>>& records) const {}

bool PostreSqlDatabase::updateRecords(
    const QString& table,
    const QString& condition,
    QHash<QString, QString>& newValues) const {}

bool PostreSqlDatabase::deleteRecords(
    const QString& table,
    const QHash<QString, QString>& searchValues) const {}

bool PostreSqlDatabase::clearTable(const QString& table) const {}

void PostreSqlDatabase::sendLog(const QString& log) const {
  if (LogEnable) {
    emit const_cast<PostreSqlDatabase*>(this)->logging(
        QString("%1 - %2").arg(objectName(), log));
  }
}

void PostreSqlDatabase::loadSettings() {
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

void PostreSqlDatabase::createDatabaseConnection() {
  QSqlDatabase postgres = QSqlDatabase::addDatabase("QPSQL", ConnectionName);

  postgres.setHostName(HostAddress.toString());
  postgres.setPort(HostPort);
  postgres.setDatabaseName(DatabaseName);
  postgres.setUserName(UserName);
  postgres.setPassword(UserPassword);
}

void PostreSqlDatabase::extractRecords(
    QSqlQuery& request,
    QVector<QSharedPointer<QHash<QString, QString>>>& records) const {
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
