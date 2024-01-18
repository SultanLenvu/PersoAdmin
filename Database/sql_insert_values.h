#ifndef SQLINSERTVALUES_H
#define SQLINSERTVALUES_H

#include <QObject>

#include "types.h"

class SqlInsertValues
{
 private:
  QVector<QString> Fields;
  QHash<QString, int32_t> FieldIndex;
  QVector<SharedVector<QString>> Values;

 public:
  SqlInsertValues();
  ~SqlInsertValues();

  void add(const StringDictionary& record);
  void add(const QString& name,
           const std::shared_ptr<QVector<QString>>& values);
  void add(const QString& field, const QString& value);
  void addField(const QString& field);

  void appendToQuery(QString& query) const;
  void clear();

 private:
  Q_DISABLE_COPY_MOVE(SqlInsertValues)
 signals:
};

#endif // SQLINSERTVALUES_H
