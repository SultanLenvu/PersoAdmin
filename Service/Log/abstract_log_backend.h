#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include <QString>

class ILogBackend {
 public:
  ILogBackend() = default;
  virtual ~ILogBackend() = default;

 public:
  virtual void writeMessage(const QString& str) = 0;

 private:
  Q_DISABLE_COPY_MOVE(ILogBackend)
};

#endif /* LOGBACKEND_H */
