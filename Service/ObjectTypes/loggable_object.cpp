#include "loggable_object.h"

LoggableObject::LoggableObject() {
  Connection = std::unique_ptr<LogSystemBundle>(new LogSystemBundle());
}

void LoggableObject::sendLog(const QString& log) const {
  emit Connection->logging(log);
}
