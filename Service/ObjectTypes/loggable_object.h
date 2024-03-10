#ifndef LOGGABLEOBJECT_H
#define LOGGABLEOBJECT_H

#include <QString>

#include "log_system_bundle.h"

class LoggableObject {
  LogSystemBundle Connection;
  QString SourceName;
  size_t ThreadId;

 public:
  explicit LoggableObject(const QString& name);
  virtual ~LoggableObject() = default;

 public:
  size_t threadId(void) const;
  QString sourceName(void) const;

 protected:
  void sendLog(const QString& log) const;
  void updateThreadId(void);

 private:
  Q_DISABLE_COPY_MOVE(LoggableObject)
};

#endif // LOGGABLEOBJECT_H
