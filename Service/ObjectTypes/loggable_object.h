#ifndef LOGGABLEOBJECT_H
#define LOGGABLEOBJECT_H

#include <QString>

#include "log_system_bundle.h"

class LoggableObject {
  LogSystemBundle Connection;
  QString LogName;
  size_t ThreadId;

 public:
  LoggableObject();
  virtual ~LoggableObject() = default;

 protected:
  void sendLog(const QString& log) const;
  size_t threadId(void) const;
  QString logName(void) const;

 protected:
  void updateThreadId(void);
  void setLogName(const QString& name);

 private:
  Q_DISABLE_COPY_MOVE(LoggableObject)
};

#endif // LOGGABLEOBJECT_H
