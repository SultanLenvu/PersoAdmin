#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPlainTextEdit>
#include <QSettings>
#include <QThread>

#include "StickerPrinter/isticker_printer.h"
#include "administration_system.h"
#include "perso_client.h"

class AdminManager : public QObject {
  Q_OBJECT

 private:
  bool LogEnable;

  AdministrationSystem* Administrator;
  QHash<AdministrationSystem::ReturnStatus, QString>
      AdministratorReturnStatusMatch;

  PersoClient* Client;
  QHash<PersoClient::ReturnStatus, QString> ClientReturnStatusMatch;

  IStickerPrinter* StickerPrinter;
  QHash<IStickerPrinter::ReturnStatus, QString> StickerPrinterReturnStatusMatch;

  QMutex Mutex;

 public:
  AdminManager(QObject* parent);
  ~AdminManager();

 public slots:
  void insctanceThreadStarted_slot(void);

  void connectDatabase(void);
  void disconnectDatabase(void);
  void showDatabaseTable(const QString& name, SqlResponseModel* model);
  void clearDatabaseTable(const QString& name, SqlResponseModel* model);
  void performCustomRequest(const QString& req, SqlResponseModel* model);

  // Заказы
  void createNewOrder(
      const QSharedPointer<QHash<QString, QString>> orderParameterseters,
      SqlResponseModel* model);
  void deleteLastOrder(SqlResponseModel* model);
  void startOrderAssembling(const QString& orderId, SqlResponseModel* model);
  void stopOrderAssembling(const QString& orderId, SqlResponseModel* model);
  void showOrderTable(SqlResponseModel* model);

  // Производственные линии
  void createNewProductionLine(const QSharedPointer<QHash<QString, QString>>
                                   productionLineParameterseters,
                               SqlResponseModel* model);
  void allocateInactiveProductionLines(const QString& orderId,
                                       SqlResponseModel* model);
  void shutdownAllProductionLines(SqlResponseModel* model);
  void deleteLastProductionLine(SqlResponseModel* model);
  void showProductionLineTable(SqlResponseModel* model);
  void linkProductionLineWithBox(
      const QSharedPointer<QHash<QString, QString>> linkParameterseters,
      SqlResponseModel* model);

  // Заказчики
  void initIssuers(SqlResponseModel* model);
  void initTransportMasterKeys(SqlResponseModel* model);
  void linkIssuerWithMasterKeys(
      SqlResponseModel* model,
      const QSharedPointer<QHash<QString, QString>> param);

  // Транспондеры
  void releaseTranspondersManually(
      const QSharedPointer<QHash<QString, QString>> param,
      SqlResponseModel* model);
  void refundTranspondersManually(
      const QSharedPointer<QHash<QString, QString>> param,
      SqlResponseModel* model);
  void shipPallets(const QSharedPointer<QHash<QString, QString>> param,
                   SqlResponseModel* model);

  // Клиент
  void releaseTransponder(const QSharedPointer<QHash<QString, QString>> param);
  void confirmTransponderRelease(
      const QSharedPointer<QHash<QString, QString>> param);
  void rereleaseTransponder(
      const QSharedPointer<QHash<QString, QString>> param);
  void confirmTransponderRerelease(
      const QSharedPointer<QHash<QString, QString>> param);
  void rollbackProductionLine(
      const QSharedPointer<QHash<QString, QString>> param);
  void printBoxStickerOnServer(QSharedPointer<QHash<QString, QString>> param);
  void printLastBoxStickerOnServer();
  void printPalletStickerOnServer(
      QSharedPointer<QHash<QString, QString>> param);
  void printLastPalletStickerOnServer();

  // Принтеры
  void printTransponderSticker(const QString& id, SqlResponseModel* model);
  void printBoxSticker(const QString& id, SqlResponseModel* model);
  void printPalletSticker(const QString& id, SqlResponseModel* model);
  void execPrinterStickerCommandScript(
      const QSharedPointer<QStringList> commandScript);

  void applySettings();

 private:
  Q_DISABLE_COPY(AdminManager)
  void loadSettings(void);
  void sendLog(const QString& log) const;

  void createAdministrator(void);
  void createClient(void);
  void createStickerPrinter(void);

  void startOperationPerforming(const QString& operationName);
  void finishOperationPerforming(const QString& operationName);

  void processAdministratorError(AdministrationSystem::ReturnStatus status,
                                 const QString& operationName);
  void processClientError(PersoClient::ReturnStatus status,
                          const QString& operationName);
  void processStickerPrinterError(IStickerPrinter::ReturnStatus status,
                                  const QString& operationName);

 signals:
  void logging(const QString& log) const;
  void notifyUser(const QString& data);
  void notifyUserAboutError(const QString& data);
  void operationPerfomingStarted(const QString& operationName);
  void operationPerformingFinished(const QString& operationName);

  void displayFirmware_signal(QSharedPointer<QFile> firmware);
  void displayTransponderData_signal(
      QSharedPointer<QHash<QString, QString>> transponderData);
};

//==================================================================================

#endif  // ADMIN_MANAGER_H
