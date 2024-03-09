#ifndef PERSOSERVERMANAGER_H
#define PERSOSERVERMANAGER_H

#include <QFile>

#include "abstract_server_connection.h"
#include "loggable_object.h"
#include "named_object.h"
#include "progressable_async_wrapper.h"

class PersoServerAsyncWrapper : public NamedObject,
                                public ProgressableAsyncWrapper,
                                public LoggableObject {
  Q_OBJECT
 private:  
  std::unique_ptr<AbstractServerConnection> Server;

  std::unique_ptr<QFile> Firmware;

  std::shared_ptr<StringDictionary> ProductionLineData;
  std::shared_ptr<StringDictionary> BoxData;
  std::shared_ptr<StringDictionary> TransponderData;

 public:
  Q_INVOKABLE explicit PersoServerAsyncWrapper(const QString& name);
  ~PersoServerAsyncWrapper();

  // Own
 public:
  void connect(void);
  void disconnect(void);

  void echo(void);
  void logOn(const std::shared_ptr<StringDictionary> param);
  void logOut(void);

  void launchProductionLine(const std::shared_ptr<StringDictionary> param);
  void shutdownProductionLine(void);
  void getProductionLineData(void);

  void requestBox(void);
  void getCurrentBoxData(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void rereleaseTransponder(const std::shared_ptr<StringDictionary> param);
  void rollbackTransponder(void);
  void getCurrentTransponderData(void);
  void getTransponderData(const std::shared_ptr<StringDictionary> param);

  void printBoxSticker(const std::shared_ptr<StringDictionary> param);
  void printLastBoxSticker(void);
  void printPalletSticker(const std::shared_ptr<StringDictionary> param);
  void printLastPalletSticker(void);

  void onServerDisconnected(void);

 private:
  ReturnStatus checkConfig(void);

 signals:
  void productionLineDataReady(const std::shared_ptr<StringDictionary> data);
  void transponderDataReady(const std::shared_ptr<StringDictionary> data);
  void boxDataReady(const std::shared_ptr<StringDictionary> data);
  void firwareReady(const std::shared_ptr<QByteArray> firmware);

  void authorizationCompleted(void);

  void printTransponderSticker_signal(const StringDictionary& data,
                                      ReturnStatus& ret);
};

#endif  // PERSOSERVERMANAGER_H
