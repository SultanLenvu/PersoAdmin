#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPlainTextEdit>
#include <QSettings>
#include <QThread>

#include "administration_system.h"
#include "isticker_printer.h"
#include "perso_client.h"
#include "types.h"

class AdminManager : public QObject {
  Q_OBJECT

 private:
  std::unique_ptr<AdministrationSystem> Administrator;
  QHash<ReturnStatus, QString> AdministratorReturnStatusMatch;

  //  PersoClient* Client;

  std::unique_ptr<IStickerPrinter> StickerPrinter;
  QHash<IStickerPrinter::ReturnStatus, QString> StickerPrinterReturnStatusMatch;

 public:
  AdminManager(const QString& name);
  ~AdminManager();

 public slots:
  void insctanceThreadStarted_slot(void);

  void connectDatabase(void);
  void disconnectDatabase(void);
  void showDatabaseTable(const QString& name, SqlQueryValues* model);
  void performCustomRequest(const QString& req, SqlQueryValues* model);

  // Заказы
  void createNewOrder(
      const std::shared_ptr<StringDictionary> orderParameterseters,
      SqlQueryValues* model);
  void startOrderAssembling(const std::shared_ptr<StringDictionary> param,
                            SqlQueryValues* model);
  void stopOrderAssembling(const std::shared_ptr<StringDictionary> param,
                           SqlQueryValues* model);
  void showOrderTable(SqlQueryValues* model);

  // Производственные линии
  void createNewProductionLine(
      const std::shared_ptr<StringDictionary> productionLineParameterseters,
      SqlQueryValues* model);
  void activateProductionLine(const std::shared_ptr<StringDictionary>,
                              SqlQueryValues* model);
  void activateAllProductionLines(SqlQueryValues* model);
  void deactivateProductionLine(const std::shared_ptr<StringDictionary>,
                                SqlQueryValues* model);
  void deactivateAllProductionLines(SqlQueryValues* model);
  void showProductionLineTable(SqlQueryValues* model);

  // Заказчики
  void initIssuers(SqlQueryValues* model);
  void initTransportMasterKeys(SqlQueryValues* model);
  void linkIssuerWithMasterKeys(const std::shared_ptr<StringDictionary> param,
                                SqlQueryValues* model);

  // Транспондеры
  void releaseTranspondersManually(
      const std::shared_ptr<StringDictionary> param,
      SqlQueryValues* model);
  void refundTranspondersManually(const std::shared_ptr<StringDictionary> param,
                                  SqlQueryValues* model);
  void shipPallets(const std::shared_ptr<StringDictionary> param,
                   SqlQueryValues* model);

  // Клиент
  void releaseTransponder(const std::shared_ptr<StringDictionary> param);
  void confirmTransponderRelease(const std::shared_ptr<StringDictionary> param);
  void rereleaseTransponder(const std::shared_ptr<StringDictionary> param);
  void confirmTransponderRerelease(
      const std::shared_ptr<StringDictionary> param);
  void rollbackProductionLine(const std::shared_ptr<StringDictionary> param);
  void printBoxStickerOnServer(std::shared_ptr<StringDictionary> param);
  void printLastBoxStickerOnServer();
  void printPalletStickerOnServer(std::shared_ptr<StringDictionary> param);
  void printLastPalletStickerOnServer();

  // Принтеры
  void printTransponderSticker(const std::shared_ptr<StringDictionary> param,
                               SqlQueryValues* model);
  void printBoxSticker(const std::shared_ptr<StringDictionary> param,
                       SqlQueryValues* model);
  void printPalletSticker(const std::shared_ptr<StringDictionary> param,
                          SqlQueryValues* model);
  void execPrinterStickerCommandScript(
      const std::shared_ptr<QStringList> commandScript);

  void applySettings();

 private:
  Q_DISABLE_COPY_MOVE(AdminManager)
  void loadSettings(void);
  void sendLog(const QString& log);

  void createAdministrator(void);
  void createClient(void);
  void createStickerPrinter(void);

  void processAdministratorError(ReturnStatus status,
                                 const QString& operationName);
  void processClientError(PersoClient::ReturnStatus status,
                          const QString& operationName);
  void processStickerPrinterError(IStickerPrinter::ReturnStatus status,
                                  const QString& operationName);

 signals:
  void logging(const QString& log);
  void executionStarted(const QString& operationName);
  void executionFinished(const QString& operationName, ReturnStatus ret);

  void displayFirmware_signal(std::shared_ptr<QFile> firmware);
  void displayTransponderData_signal(
      std::shared_ptr<StringDictionary> transponderData);
};

//==================================================================================

#endif  // ADMIN_MANAGER_H
