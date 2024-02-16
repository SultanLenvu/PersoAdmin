#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include "psobject.h"

class LogBackend : public PSObject {
  Q_OBJECT

 public:
  LogBackend(const QString& name);
  virtual ~LogBackend();

  virtual void writeLogMessage(const QString& str) = 0;
};

#endif /* LOGBACKEND_H */
