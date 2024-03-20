#ifndef PROGRAMMERASYNCWRAPPER_H
#define PROGRAMMERASYNCWRAPPER_H

#include "i_async_wrapper.h"
#include "progressable_async_wrapper.h"

class ProgrammerAsyncWrapper : public ProgressableAsyncWrapper {
 public:
  Q_INVOKABLE explicit ProgrammerAsyncWrapper(const QString& name);
  ~ProgrammerAsyncWrapper();
};

#endif  // PROGRAMMERASYNCWRAPPER_H
