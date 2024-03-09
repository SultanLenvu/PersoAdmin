#ifndef PROGRAMMERASYNCWRAPPER_H
#define PROGRAMMERASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "loggable_object.h"
#include "named_object.h"
#include "progressable_async_wrapper.h"

class ProgrammerAsyncWrapper : public NamedObject,
                               public ProgressableAsyncWrapper,
                               public LoggableObject {
 public:
  Q_INVOKABLE explicit ProgrammerAsyncWrapper(const QString& name);
  ~ProgrammerAsyncWrapper();
};

#endif  // PROGRAMMERASYNCWRAPPER_H
