#ifndef SQLTABLE_H
#define SQLTABLE_H

#include <QHash>
#include <QScopedPointer>
#include <QString>
#include <QtSql>

// #include "sql_table_relation.h"

class SqlTable {
 private:
  QString Name;

  QString PrimaryKey;
  QVector<QString> Columns;
  QHash<QString, QString> Relations;

  uint32_t RecordCount;

 public:
  // Rule of 6
  explicit SqlTable(const QString& name);
  ~SqlTable();

  explicit SqlTable(const SqlTable& other);
  SqlTable& operator=(const SqlTable& other);

  explicit SqlTable(SqlTable&& other) noexcept;
  SqlTable& operator=(SqlTable&& other) noexcept;

  // Accessors
  QString getName() const;
  void setName(const QString& newName);

  QString getPrimaryKey() const;
  void setPrimaryKey(const QString& newPrimaryKey);

  uint32_t getRecordCount() const;
  void setRecordCount(uint32_t newRecordCount);

  QVector<QString>* columns();
  QHash<QString, QString>* relations();

  // Functionality
  bool init(QSqlDatabase* db);

 signals:
};

#endif  // SQLTABLE_H
