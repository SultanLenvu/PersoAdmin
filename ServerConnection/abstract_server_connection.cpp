#include "abstract_server_connection.h"

AbstractServerConnection::AbstractServerConnection(const QString& name)
    : PObject{name} {
  connectDepedencies();
}

AbstractServerConnection::~AbstractServerConnection() {}

void AbstractServerConnection::connectDepedencies() {}
