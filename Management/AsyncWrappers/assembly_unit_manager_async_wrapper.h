#ifndef ASSEMBLYUNITMANAGERASYNCWRAPPER_H
#define ASSEMBLYUNITMANAGERASYNCWRAPPER_H

#include "assembly_unit_manager.h"
#include "progressable_async_wrapper.h"

class AssemblyUnitManagerAsyncWrapper : public ProgressableAsyncWrapper {
  Q_OBJECT

 private:
  std::unique_ptr<AssemblyUnitManager> Manager;

 public:
  Q_INVOKABLE explicit AssemblyUnitManagerAsyncWrapper(const QString& name);
  ~AssemblyUnitManagerAsyncWrapper() = default;

 public slots:
  void logOn(const StringDictionary& param);
  void logOut(void);

  void requestBox(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(const StringDictionary& param);
  void rollbackTransponder(void);

 private:
  Q_DISABLE_COPY_MOVE(AssemblyUnitManagerAsyncWrapper)
};

#endif // ASSEMBLYUNITMANAGERASYNCWRAPPER_H
