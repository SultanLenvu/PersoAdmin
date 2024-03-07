#ifndef LOGGABLEOBJECT_H
#define LOGGABLEOBJECT_H

#include <QString>

#include "log_system_bundle.h"

class LoggableObject {
  std::unique_ptr<LogSystemBundle> Connection;

 public:
  LoggableObject();
  virtual ~LoggableObject() = default;

 protected:
  void sendLog(const QString& log) const;

 private:
  Q_DISABLE_COPY_MOVE(LoggableObject)
};

#endif // LOGGABLEOBJECT_H
