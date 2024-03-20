#ifndef PERSOSERVERMANAGER_H
#define PERSOSERVERMANAGER_H

#include <QFile>

#include "i_server_connection.h"
#include "progressable_async_wrapper.h"

class ServerConnectionAsyncWrapper : public ProgressableAsyncWrapper {
  Q_OBJECT
 private:
  std::shared_ptr<IServerConnection> Server;

 public:
  Q_INVOKABLE explicit ServerConnectionAsyncWrapper(const QString& name);
  ~ServerConnectionAsyncWrapper() = default;

 public:
  void connect(void);
  void disconnect(void);

  void echo(void);

  void launchProductionLine(const StringDictionary& param);
  void shutdownProductionLine(void);
  void getProductionLineData(void);

  void requestBox(void);
  void getCurrentBoxData(void);
  void refundCurrentBox(void);
  void completeCurrentBox(void);

  void releaseTransponder(void);
  void confirmTransponderRelease(const StringDictionary& param);
  void rereleaseTransponder(const StringDictionary& param);
  void confirmTransponderRerelease(
      const StringDictionary& param);
  void rollbackTransponder(void);
  void getCurrentTransponderData(void);
  void getTransponderData(const StringDictionary& param);

  void printBoxSticker(const StringDictionary& param);
  void printLastBoxSticker(void);
  void printPalletSticker(const StringDictionary& param);
  void printLastPalletSticker(void);

 private:
  Q_DISABLE_COPY_MOVE(ServerConnectionAsyncWrapper)
  ReturnStatus checkConfig(void);
  void connectDependecies(void);

 signals:
  void productionLineDataReady(const StringDictionary& data);
  void transponderDataReady(const StringDictionary& data);
  void boxDataReady(const StringDictionary& data);
  void firwareReady(const QStringList& firmware);
};

#endif  // PERSOSERVERMANAGER_H
