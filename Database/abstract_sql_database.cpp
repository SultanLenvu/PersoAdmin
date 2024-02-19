#include "abstract_sql_database.h"

AbstractSqlDatabase::AbstractSqlDatabase(const QString& name)
    : PSObject{name} {}

AbstractSqlDatabase::~AbstractSqlDatabase() {}
