#ifndef SQLTABLE_H
#define SQLTABLE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QtSql>

#include "abstract_sql_table.h"

class PostgreSqlTable : public AbstractSqlTable {
 private:
  QString TableName;
  QString ConnectionName;

  QString PrimaryKey;
  QVector<QString> Columns;
  StringDictionary Relations;

  uint32_t RecordMaxCount;
  QString CurrentOrder;

 public:
  explicit PostgreSqlTable(const QString& name,
                           const QString& tableName,
                           const QString& connectionName);
  ~PostgreSqlTable();

  // PSObject interface
 public:
  virtual void applySettings() override;

  // AbstractSqlTable interface
 public:
  virtual bool init() override;

  // CRUD
  virtual bool createRecords(const SqlQueryValues& response) override;
  virtual bool readRecords(SqlQueryValues& response) override;
  virtual bool readRecords(const QString& conditions,
                           SqlQueryValues& response) override;
  virtual bool readLastRecord(SqlQueryValues& response) override;
  virtual bool updateRecords(const SqlQueryValues& newValues) override;
  virtual bool updateRecords(const QString& condition,
                             const SqlQueryValues& newValues) override;
  virtual bool deleteRecords(const QString& condition) override;
  virtual bool clear() override;

  // Aggregation
  virtual bool getRecordCount(uint32_t& count) override;

  // Own
 public:
  QString getPrimaryKey() const;
  void setPrimaryKey(const QString& newPrimaryKey);

  uint32_t getRecordMaxCount() const;
  void setRecordMaxCount(uint32_t newRecordMaxCount);

  Qt::SortOrder getCurrentOrder() const;
  void setCurrentOrder(Qt::SortOrder order);

  const QVector<QString>* columns() const;
  const StringDictionary* relations() const;

 private:
  void loadSettings(void);

  bool checkFieldNames(const SqlQueryValues& record) const;
  bool checkFieldNames(const StringDictionary& record) const;

 signals:
};

#endif  // SQLTABLE_H
