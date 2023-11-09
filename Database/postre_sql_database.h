#ifndef PostgreSqlDatabase_H
#define PostgreSqlDatabase_H

#include <QHostAddress>
#include <QSharedPointer>
#include <QVector>
#include <QtSql>

#include "abstract_sql_database.h"
#include "sql_table.h"

class PostgreSqlDatabase : public AbstractSqlDatabase {
  Q_OBJECT

 private:
  bool LogEnable;
  Qt::SortOrder CurrentOrder;
  uint32_t RecordMaxCount;

  QString ConnectionName;
  QString DatabaseName;

  QHostAddress HostAddress;
  uint32_t HostPort;

  QString UserName;
  QString UserPassword;

  QHash<QString, QSharedPointer<SqlTable>> Tables;

 public:
  explicit PostgreSqlDatabase(QObject* parent, const QString& connectionName);
  ~PostgreSqlDatabase();

  // AbstractSqlDatabase interface
  virtual void applySettings() override;

  virtual bool connect() override;
  virtual void disconnect() override;
  virtual bool isConnected() override;

  virtual bool openTransaction() const override;
  virtual bool commitTransaction() const override;
  virtual bool rollbackTransaction() const override;

  virtual Qt::SortOrder getCurrentOrder() const override;
  virtual void setCurrentOrder(Qt::SortOrder order) override;

  virtual uint32_t getRecordMaxCount(void) const override;
  virtual void setRecordMaxCount(uint32_t count) override;

  virtual bool execCustomRequest(
      const QString& requestText,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const override;

  // Create
  virtual bool createRecord(
      const QString& table,
      QVector<QHash<QString, QString>>& records) const override;

  // Read
  virtual bool readRecords(
      const QString& table,
      const QHash<QString, QString>& searchValues,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const override;
  virtual bool readMergedRecords(
      const QStringList& tables,
      const QHash<QString, QString>& searchValues,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const override;

  // Update
  virtual bool updateRecords(const QString& table,
                             const QString& condition,
                             QHash<QString, QString>& newValues) const override;

  // Delete
  virtual bool deleteRecords(
      const QString& table,
      const QHash<QString, QString>& searchValues) const override;
  virtual bool clearTable(const QString& table) const override;

 private:
  Q_DISABLE_COPY_MOVE(PostgreSqlDatabase)
  void sendLog(const QString& log) const;
  void loadSettings(void);

  void createDatabaseConnection(void);
  bool init(void);
  bool createTable(void);

  bool checkTableName(const QString& name);
  bool checkTableField(const QString& field);

  bool getTableComlumnNames(const QString& name);

  void extractRecords(
      QSqlQuery& request,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const;
};

#endif  // PostgreSqlDatabase_H
