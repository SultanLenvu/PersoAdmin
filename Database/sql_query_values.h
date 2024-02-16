#ifndef SQL_QUERY_VALUES_H
#define SQL_QUERY_VALUES_H

#include <QAbstractTableModel>
#include <QHash>
#include <QObject>
#include <QSet>

#include <QSqlQuery>
#include <QVector>

#include "types.h"

class SqlQueryValues final {
 private:
  QVector<QString> Fields;
  QHash<QString, int32_t> FieldIndex;
  QVector<SharedVector<QString>> Values;

 public:
  explicit SqlQueryValues();
  ~SqlQueryValues();

  SqlQueryValues(const SqlQueryValues& other);
  SqlQueryValues(SqlQueryValues&& other) noexcept;
  SqlQueryValues& operator=(const SqlQueryValues& other);
  SqlQueryValues& operator=(SqlQueryValues&& other) noexcept;

  QString fieldName(uint32_t i) const;
  QString get(uint32_t record, const QString& field) const;
  QString get(uint32_t record, uint32_t field) const;
  QString get(uint32_t field) const;
  QString get(const QString& field) const;
  QString getLast(const QString& field) const;
  int32_t recordCount(void) const;
  int32_t fieldCount(void) const;
  bool isEmpty(void) const;
  void appendToInsert(QString& queryText) const;

  void extractRecords(QSqlQuery& request);
  void add(const StringDictionary& record);
  void add(const QString& name, const std::shared_ptr<QVector<QString>>& values);
  void add(const QString& field, const QString& value);
  void addField(const QString& field);
  void clear();
};

#endif  // SQL_QUERY_VALUES_H
