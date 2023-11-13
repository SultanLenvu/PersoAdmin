#ifndef SQLTABLE_H
#define SQLTABLE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QtSql>

#include "abstract_sql_table.h"

class PostgreSqlTable : public AbstractSqlTable {
 private:
  bool LogEnable;

  QString ConnectionName;

  QString PrimaryKey;
  QVector<QString> Columns;
  QHash<QString, QString> Relations;

  uint32_t RecordMaxCount;
  QString CurrentOrder;

 public:
  explicit PostgreSqlTable(const QString& name, const QString& connectionName);
  ~PostgreSqlTable();

  // Accessors
  QString getPrimaryKey() const;
  void setPrimaryKey(const QString& newPrimaryKey);

  uint32_t getRecordMaxCount() const;
  void setRecordMaxCount(uint32_t newRecordMaxCount);

  Qt::SortOrder getCurrentOrder() const;
  void setCurrentOrder(Qt::SortOrder order);

  const QVector<QString>* columns() const;
  const QHash<QString, QString>* relations() const;

  // AbstractSqlTable interface
 public:
  virtual bool init() override;
  virtual void applySettings() override;
  virtual bool createRecords(
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const override;
  virtual bool readRecords(
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const override;
  virtual bool readRecords(
      const QString& conditions,
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const override;
  virtual bool readLastRecord(QHash<QString, QString>& record) const override;
  virtual bool updateRecords(
      const QString& condition,
      const QHash<QString, QString>& newValues) const override;
  virtual bool deleteRecords(const QString& condition) const override;
  virtual bool clear() const override;

 private:
  Q_DISABLE_COPY_MOVE(PostgreSqlTable)
  void sendLog(const QString& log) const;
  void loadSettings(void);

  bool checkFieldNames(
      const QHash<QString, QSharedPointer<QVector<QString>>>& record) const;
  bool checkFieldNames(const QHash<QString, QString>& record) const;

  void extractRecord(QSqlQuery& request, QHash<QString, QString>& record) const;
  void extractRecords(
      QSqlQuery& request,
      QHash<QString, QSharedPointer<QVector<QString>>>& records) const;

 signals:
};

#endif  // SQLTABLE_H
