#ifndef ABSTRACTRETURNSTATUSHANDLER_H
#define ABSTRACTRETURNSTATUSHANDLER_H

#include "named_object.h"
#include "types.h"

class AbstractReturnStatusHandler : public NamedObject
{
  Q_OBJECT
 public:
  explicit AbstractReturnStatusHandler(const QString& name);
  virtual ~AbstractReturnStatusHandler();

 public:
  virtual void handle(ReturnStatus ret) = 0;
};

#endif // ABSTRACTRETURNSTATUSHANDLER_H
