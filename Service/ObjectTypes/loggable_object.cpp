#include "loggable_object.h"

#include <QDebug>
#include <QThread>

LoggableObject::LoggableObject(const QString& name)
    : SourceName(name),
      ThreadId(reinterpret_cast<size_t>(QThread::currentThreadId())) {}

void LoggableObject::sendLog(const QString& log) const {
  emit Connection.logging(log, this);
}

size_t LoggableObject::threadId() const {
  return ThreadId;
}

QString LoggableObject::sourceName() const {
  return SourceName;
}

void LoggableObject::updateThreadId() {
  ThreadId = reinterpret_cast<size_t>(QThread::currentThreadId());
}
