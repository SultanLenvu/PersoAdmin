#include "abstract_sql_database.h"

AbstractSqlDatabase::AbstractSqlDatabase(const QString& name)
    : ConfigurableObject{name} {}

AbstractSqlDatabase::~AbstractSqlDatabase() {}
