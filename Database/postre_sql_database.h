#ifndef POSTRESQLDATABASE_H
#define POSTRESQLDATABASE_H

#include <QHostAddress>
#include <QScopedPointer>
#include <QVector>
#include <QtSql>

#include "abstract_sql_database.h"
#include "sql_table.h"

class PostreSqlDatabase : public AbstractSqlDatabase {
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

  QVector<QScopedPointer<SqlTable>> Tables;

 public:
  explicit PostreSqlDatabase(QObject* parent, const QString& connectionName);
  ~PostreSqlDatabase();

  // AbstractSqlDatabase interface
  virtual void applySettings() override;

  virtual bool connect() override;
  virtual void disconnect() override;
  virtual bool isConnected() override;

  virtual bool openTransaction() const override;
  virtual bool closeTransaction() const override;
  virtual bool abortTransaction() const override;

  virtual Qt::SortOrder getCurrentOrder() const override;
  virtual void setCurrentOrder(Qt::SortOrder order) override;

  virtual uint32_t getRecordMaxCount(void) const override;
  virtual void setRecordMaxCount(uint32_t count) const override;

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
  Q_DISABLE_COPY_MOVE(PostreSqlDatabase)
  void sendLog(const QString& log) const;
  void loadSettings(void);
  void createDatabaseConnection(void);
  void extractRecords(
      QSqlQuery& request,
      QVector<QSharedPointer<QHash<QString, QString>>>& records) const;
};

#endif  // POSTRESQLDATABASE_H
