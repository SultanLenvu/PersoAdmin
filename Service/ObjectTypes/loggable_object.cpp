#include "loggable_object.h"

#include <QDebug>
#include <QThread>

LoggableObject::LoggableObject()
    : ThreadId(reinterpret_cast<size_t>(QThread::currentThreadId())) {}

void LoggableObject::sendLog(const QString& log) const {
  emit Connection.logging(log, this);
}

size_t LoggableObject::threadId() const {
  return ThreadId;
}

QString LoggableObject::logName() const {
  return LogName;
}

void LoggableObject::updateThreadId() {
  ThreadId = reinterpret_cast<size_t>(QThread::currentThreadId());
}

void LoggableObject::setLogName(const QString& name) {
  LogName = name;
}
