#ifndef ABSTRACTSQLTABLE_H
#define ABSTRACTSQLTABLE_H

#include <QObject>

#include "psobject.h"
#include "sql_query_values.h"

class AbstractSqlTable : public PSObject {
  Q_OBJECT

 public:
  explicit AbstractSqlTable(const QString& name);
  virtual ~AbstractSqlTable();

  virtual bool init() = 0;
  virtual void applySettings() = 0;

  // Create
  virtual bool createRecords(const SqlQueryValues& records) = 0;

  // Read
  virtual bool readRecords(SqlQueryValues& records) = 0;
  virtual bool readRecords(const QString& conditions,
                           SqlQueryValues& records) = 0;
  virtual bool readLastRecord(SqlQueryValues& record) = 0;

  // Update
  virtual bool updateRecords(const SqlQueryValues& newValues) = 0;
  virtual bool updateRecords(const QString& condition,
                             const SqlQueryValues& newValues) = 0;

  // Delete
  virtual bool deleteRecords(const QString& condition) = 0;
  virtual bool clear(void) = 0;

  // Aggregation
  virtual bool getRecordCount(uint32_t& count) = 0;

 signals:
};

#endif  // ABSTRACTSQLTABLE_H
