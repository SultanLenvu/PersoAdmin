#include "abstract_sql_database.h"

AbstractSqlDatabase::AbstractSqlDatabase(const QString& name)
    : PObject{name} {}

AbstractSqlDatabase::~AbstractSqlDatabase() {}
