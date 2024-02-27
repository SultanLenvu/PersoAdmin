#ifndef ABSTRACTRETURNSTATUSHANDLER_H
#define ABSTRACTRETURNSTATUSHANDLER_H

#include "pobject.h"
#include "types.h"

class AbstractReturnStatusHandler : public PObject
{
  Q_OBJECT
 public:
  explicit AbstractReturnStatusHandler(const QString& name);
  virtual ~AbstractReturnStatusHandler();

 public:
  virtual void handle(ReturnStatus ret) = 0;
};

#endif // ABSTRACTRETURNSTATUSHANDLER_H
