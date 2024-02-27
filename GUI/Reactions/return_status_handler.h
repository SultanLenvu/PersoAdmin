#ifndef RETURNSTATUSHANDLER_H
#define RETURNSTATUSHANDLER_H

#include "abstract_return_status_handler.h"

class ReturnStatusHandler final : public AbstractReturnStatusHandler {
  Q_OBJECT
 public:
  ReturnStatusHandler(const QString& name);
  ~ReturnStatusHandler();

  // AbstractReturnStatusHandler interface
 public:
  virtual void handle(ReturnStatus ret) override;
};

#endif // RETURNSTATUSHANDLER_H
