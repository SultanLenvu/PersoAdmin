#ifndef PROGRAMMERASYNCWRAPPER_H
#define PROGRAMMERASYNCWRAPPER_H

#include "abstract_async_wrapper.h"

class ProgrammerAsyncWrapper : public AbstractAsyncWrapper {
  Q_OBJECT
 public:
  explicit ProgrammerAsyncWrapper(const QString& name);
  ~ProgrammerAsyncWrapper();

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;
};

#endif  // PROGRAMMERASYNCWRAPPER_H
