#include "sql_insert_values.h"

SqlInsertValues::SqlInsertValues()
{
  
}

SqlInsertValues::~SqlInsertValues() {}

void SqlInsertValues::add(const StringDictionary& record) {}

void SqlInsertValues::add(const QString& name,
                          const std::shared_ptr<QVector<QString> >& values) {}

void SqlInsertValues::add(const QString& field, const QString& value) {}

void SqlInsertValues::addField(const QString& field) {}

void SqlInsertValues::appendToQuery(QString& query) const {
  for (int32_t i = 0; i < Values.first()->size(); i++) {
    query.append("(");
    for (int32_t j = 0; j < Values.size(); j++) {
      if (Values.at(j)->at(i) != "NULL") {
        query.append(QString("'%1'").arg(Values.at(j)->at(i)));
      } else {
        query.append(QString("NULL"));
      }
      query.append(", ");
    }
    query.chop(2);
    query.append("),\n");
  }
  query.chop(2);
}

void SqlInsertValues::clear() {}
