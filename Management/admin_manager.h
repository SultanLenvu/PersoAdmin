#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QThread>

#include "Database/database_controller.h"
#include "Database/database_table_model.h"
#include "Database/postgres_controller.h"
#include "StickerPrinter/isticker_printer.h"
#include "StickerPrinter/te310_printer.h"
#include "Transponder/transponder_seed_model.h"
#include "administration_system.h"

class AdminManager : public QObject {
  Q_OBJECT

 private:
  bool LogEnable;

  AdministrationSystem* Administrator;
  QMap<AdministrationSystem::ReturnStatus, QString>
      AdministratorReturnStatusMatch;

  IStickerPrinter* StickerPrinter;
  QMap<IStickerPrinter::ReturnStatus, QString> StickerPrinterReturnStatusMatch;

  QMutex Mutex;

 public:
  AdminManager(QObject* parent);
  ~AdminManager();

 public slots:
  void applySettings();
  void on_InsctanceThreadStarted_slot(void);

  void connectDatabase(void);
  void disconnectDatabase(void);
  void showDatabaseTable(const QString& name, DatabaseTableModel* model);
  void clearDatabaseTable(const QString& name, DatabaseTableModel* model);
  void performCustomRequest(const QString& req, DatabaseTableModel* model);

  void createNewOrder(
      const QSharedPointer<QMap<QString, QString> > orderParameterseters,
      DatabaseTableModel* model);
  void deleteLastOrder(DatabaseTableModel* model);
  void startOrderAssembling(const QString& orderId, DatabaseTableModel* model);
  void stopOrderAssembling(const QString& orderId, DatabaseTableModel* model);
  void showOrderTable(DatabaseTableModel* model);

  void createNewProductionLine(
      const QMap<QString, QString>* productionLineParameterseters,
      DatabaseTableModel* model);
  void allocateInactiveProductionLines(const QString& orderId,
                                       DatabaseTableModel* model);
  void shutdownAllProductionLines(DatabaseTableModel* model);
  void deleteLastProductionLine(DatabaseTableModel* model);
  void showProductionLineTable(DatabaseTableModel* model);
  void linkProductionLineWithBox(
      const QMap<QString, QString>* linkParameterseters,
      DatabaseTableModel* model);

  void initIssuers(DatabaseTableModel* model);
  void initTransportMasterKeys(DatabaseTableModel* model);
  void linkIssuerWithMasterKeys(DatabaseTableModel* model,
                                const QMap<QString, QString>* Parameterseters);

  void printTransponderSticker(const QString& id, DatabaseTableModel* model);
  void printBoxSticker(const QString& id, DatabaseTableModel* model);
  void printPalletSticker(const QString& id, DatabaseTableModel* model);
  void execPrinterStickerCommandScript(
      const QSharedPointer<QStringList> commandScript);

 private:
  Q_DISABLE_COPY(AdminManager)
  void loadSettings(void);
  void sendLog(const QString& log) const;

  void createAdministrator(void);
  void createStickerPrinter(void);

  void startOperationPerforming(const QString& operationName);
  void finishOperationPerforming(const QString& operationName);

  void processAdministratorError(AdministrationSystem::ReturnStatus status,
                                 const QString& operationName);
  void processStickerPrinterError(IStickerPrinter::ReturnStatus status,
                                  const QString& operationName);

 private slots:
  void proxyLogging(const QString& log);

 signals:
  void logging(const QString& log) const;
  void notifyUser(const QString& data);
  void notifyUserAboutError(const QString& data);
  void operationPerfomingStarted(const QString& operationName);
  void operationPerformingFinished(const QString& operationName);
};

//==================================================================================

#endif  // ADMIN_MANAGER_H
