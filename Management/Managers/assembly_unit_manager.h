#ifndef ASSEMBLYUNITMANAGER_H
#define ASSEMBLYUNITMANAGER_H

#include "named_object.h"
#include "types.h"

class AssemblyUnitManager : public NamedObject {
  Q_OBJECT
 public:
  explicit AssemblyUnitManager(const QString& name);
  ~AssemblyUnitManager() = default;

 public:
  ReturnStatus logOn(const QString& param);
  ReturnStatus logOut(void);

  ReturnStatus requestBox(void);
  ReturnStatus refundCurrentBox(void);
  ReturnStatus completeCurrentBox(void);

  ReturnStatus releaseTransponder(void);
  ReturnStatus rereleaseTransponder(const QString& param);
  ReturnStatus rollbackTransponder(void);

 private:
  Q_DISABLE_COPY_MOVE(AssemblyUnitManager)

  void connectDependecies(void);

 signals:
  void productionLineDataReady(const std::shared_ptr<StringDictionary> data);
  void transponderDataReady(const std::shared_ptr<StringDictionary> data);
  void boxDataReady(const std::shared_ptr<StringDictionary> data);
  void firwareReady(const std::shared_ptr<QByteArray> firmware);

  void authorizationCompleted(void);

  void printTransponderSticker_signal(const StringDictionary& data,
                                      ReturnStatus& ret);
};

#endif // ASSEMBLYUNITMANAGER_H
