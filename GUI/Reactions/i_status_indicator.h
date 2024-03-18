#ifndef ISTATUSHANDLER_H
#define ISTATUSHANDLER_H

#include "types.h"

class IStatusIndicator {
 public:
  explicit IStatusIndicator() = default;
  virtual ~IStatusIndicator() = default;

 public:
  virtual void handle(ReturnStatus ret) = 0;
};

#endif  // ISTATUSHANDLER_H
