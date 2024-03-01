#ifndef INTERNALSERVICESPACE_H
#define INTERNALSERVICESPACE_H

#include <QThread>

#include "abstract_service_space.h"
#include "log_system.h"

class InternalServiceSpace final : public AbstractServiceSpace {
  Q_OBJECT
 private:
  std::unique_ptr<QThread> Thread;

  std::unique_ptr<LogSystem> Logger;

 public:
  InternalServiceSpace(const QString& name);
  ~InternalServiceSpace();

 private:
  Q_DISABLE_COPY_MOVE(InternalServiceSpace)
};

#endif  // INTERNALSERVICESPACE_H
