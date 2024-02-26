#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include "configurable_object.h"

class LogBackend : public ConfigurableObject {
  Q_OBJECT

 public:
  LogBackend(const QString& name);
  virtual ~LogBackend();

  virtual void writeMessage(const QString& str) = 0;
};

#endif /* LOGBACKEND_H */
