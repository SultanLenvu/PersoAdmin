#include "abstract_server_connection.h"

AbstractServerConnection::AbstractServerConnection(const QString& name)
    : PSObject{name} {
  connectDepedencies();
}

AbstractServerConnection::~AbstractServerConnection() {}

void AbstractServerConnection::connectDepedencies() {}
