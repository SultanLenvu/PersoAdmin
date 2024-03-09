#ifndef ABSTRACTRETURNSTATUSHANDLER_H
#define ABSTRACTRETURNSTATUSHANDLER_H

#include "types.h"

class AbstractReturnStatusHandler {
 public:
  explicit AbstractReturnStatusHandler() = default;
  virtual ~AbstractReturnStatusHandler() = default;

 public:
  virtual void handle(ReturnStatus ret) = 0;
};

#endif // ABSTRACTRETURNSTATUSHANDLER_H
