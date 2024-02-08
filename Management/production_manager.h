#ifndef ProductionManager_H
#define ProductionManager_H

#include <QFile>

#include "abstract_manager.h"
#include "abstract_server_connection.h"

class ProductionManager : public AbstractManager {
  Q_OBJECT

 private:  
  std::unique_ptr<AbstractServerConnection> Server;

  std::unique_ptr<QFile> Firmware;

  StringDictionary ProductionLineData;
  StringDictionary BoxData;
  StringDictionary TransponderData;

 public:
  explicit ProductionManager(const QString& name);
  ~ProductionManager();

  // AbstractManager interface
 public:
  virtual void onInstanceThreadStarted(void) override;
  virtual Type type() const override;
  virtual void applySettings(void) override;

 public:
  void connectToServer(void);
  void disconnectFromServer(void);
  void launchProductionLine(const std::shared_ptr<StringDictionary> param);
  void shutdownProductionLine(void);
  void getProductionLineData(void);

  void logOnServer(const std::shared_ptr<StringDictionary> param);
  void logOutServer(void);
  void echoServer(void);

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
  void loadSettings(void);
  ReturnStatus checkConfig(void);

  void createServerConnection(void);

 signals:
  void displayProductionLineData_signal(const StringDictionary& data);
  void displayTransponderData_signal(const StringDictionary& data);
  void displayBoxData_signal(const StringDictionary& data);
  void authorizationCompleted(void);

  void printTransponderSticker_signal(const StringDictionary& data,
                                      ReturnStatus& ret);
};

#endif  // ProductionManager_H
