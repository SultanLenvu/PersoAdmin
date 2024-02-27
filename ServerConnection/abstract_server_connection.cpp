#include "abstract_server_connection.h"

AbstractServerConnection::AbstractServerConnection(const QString& name)
    : ConfigurableObject{name} {
  connectDepedencies();
}

AbstractServerConnection::~AbstractServerConnection() {}

void AbstractServerConnection::connectDepedencies() {}
