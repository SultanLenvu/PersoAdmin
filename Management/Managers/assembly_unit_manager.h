#ifndef ASSEMBLYUNITMANAGER_H
#define ASSEMBLYUNITMANAGER_H

#include "i_server_connection.h"
#include "loggable_object.h"
#include "named_object.h"
#include "types.h"

class AssemblyUnitManager : public NamedObject, public LoggableObject {
  Q_OBJECT
 private:
  std::shared_ptr<IServerConnection> Server;

 public:
  explicit AssemblyUnitManager(const QString& name);
  ~AssemblyUnitManager() = default;

 public:
  ReturnStatus logOn(const StringDictionary& param);
  ReturnStatus logOut(void);

  ReturnStatus requestBox(void);
  ReturnStatus refundCurrentBox(void);
  ReturnStatus completeCurrentBox(void);

  ReturnStatus releaseTransponder(void);
  ReturnStatus rereleaseTransponder(const StringDictionary& param);
  ReturnStatus rollbackTransponder(void);

 private:
  Q_DISABLE_COPY_MOVE(AssemblyUnitManager)
  void connectDependecies(void);

 signals:
  void productionLineDataReady(const StringDictionary& data);
  void transponderDataReady(const StringDictionary& data);
  void boxDataReady(const StringDictionary& data);
  void firwareReady(const QStringList& firmware);

  void authorizationCompleted(void);

  void printTransponderSticker_signal(const StringDictionary& data,
                                      ReturnStatus& ret);
};

#endif // ASSEMBLYUNITMANAGER_H
