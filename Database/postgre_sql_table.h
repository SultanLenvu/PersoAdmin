#ifndef SQLTABLE_H
#define SQLTABLE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QtSql>

#include "abstract_sql_table.h"

class PostgreSqlTable : public AbstractSqlTable {
 private:
  QString ConnectionName;

  QString PrimaryKey;
  QVector<QString> Columns;
  QHash<QString, QString> Relations;

  uint32_t RecordCount;

 public:
  explicit PostgreSqlTable(const QString& name, const QString& connectionName);
  ~PostgreSqlTable();

  // Accessors
  QString getPrimaryKey() const;
  void setPrimaryKey(const QString& newPrimaryKey);

  uint32_t getRecordCount() const;
  void setRecordCount(uint32_t newRecordCount);

  QVector<QString>* columns();
  QHash<QString, QString>* relations();

  // AbstractSqlTable interface
 public:
  virtual bool init() override;
  virtual bool createRecords(QVector<QSharedPointer<QHash<QString, QString> > >&
                                records) const override;
  virtual bool readRecords(const QHash<QString, QString>& searchValues,
                           QVector<QSharedPointer<QHash<QString, QString> > >&
                               records) const override;
  virtual bool updateRecords(const QString& condition,
                             QHash<QString, QString>& newValues) const override;
  virtual bool deleteRecords(const QString& condition) const override;
  virtual bool clearTable() const override;

 private:
  Q_DISABLE_COPY_MOVE(PostgreSqlTable)

 signals:
};

#endif  // SQLTABLE_H
