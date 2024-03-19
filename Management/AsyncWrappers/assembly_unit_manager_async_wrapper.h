#ifndef ASSEMBLYUNITMANAGERASYNCWRAPPER_H
#define ASSEMBLYUNITMANAGERASYNCWRAPPER_H

#include "progressable_async_wrapper.h"

class AssemblyUnitManagerAsyncWrapper : public ProgressableAsyncWrapper
{
  Q_OBJECT
 public:
  explicit AssemblyUnitManagerAsyncWrapper(const QString& name);
  ~AssemblyUnitManagerAsyncWrapper() = default;

 public slots:
  void logOn(const std::shared_ptr<StringDictionary> param);
  void logOut(void);

  void requestBox(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(const std::shared_ptr<StringDictionary> param);
  void rollbackTransponder(void);

 private:
  Q_DISABLE_COPY_MOVE(AssemblyUnitManagerAsyncWrapper)
};

#endif // ASSEMBLYUNITMANAGERASYNCWRAPPER_H
