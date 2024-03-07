#ifndef LOGBACKEND_H
#define LOGBACKEND_H

#include <QString>

class AbstractLogBackend {
 public:
  AbstractLogBackend() = default;
  virtual ~AbstractLogBackend() = default;

 public:
  virtual void writeMessage(const QString& str) = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractLogBackend)
};

#endif /* LOGBACKEND_H */
