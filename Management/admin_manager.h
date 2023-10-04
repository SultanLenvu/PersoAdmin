#ifndef ADMIN_MANAGER_H
#define ADMIN_MANAGER_H

#include <QElapsedTimer>
#include <QList>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QTcpServer>
#include <QThread>
#include <QTimer>
#include <QVector>

#include "Database/database_controller.h"
#include "Database/database_table_model.h"
#include "Database/postgres_controller.h"
#include "Transponder/transponder_seed_model.h"
#include "administration_system.h"
#include "administration_system_builder.h"

class AdminManager : public QObject {
  Q_OBJECT

 public:
  enum OperationState { Ready, WaitingExecution, Failed, Completed };

 private:
  OperationState CurrentState;
  QString NotificarionText;

  AdministrationSystem* Administrator;
  AdministrationSystemBuilder* AdministratorBuilder;
  QThread* AdministratorThread;

  QEventLoop* WaitingLoop;
  QTimer* ODTimer;
  QTimer* ODQTimer;
  QElapsedTimer* ODMeter;

 public:
  AdminManager(QObject* parent);
  ~AdminManager();

  void applySettings();

  void performAuthorization(
      const std::shared_ptr<QMap<QString, QString>> authDataPtr);
  void connectDatabaseManually(void);
  void disconnectDatabaseManually(void);
  void showDatabaseTable(const QString& name, DatabaseTableModel* model);
  void clearDatabaseTable(const QString& name, DatabaseTableModel* model);

  void performCustomRequest(const QString& req, DatabaseTableModel* model);

  void createNewOrder(const QMap<QString, QString>* orderParameterseters,
                      DatabaseTableModel* model);
  void startOrderAssemblingManually(const QString& orderId,
                                    DatabaseTableModel* model);
  void stopOrderAssemblingManually(const QString& orderId,
                                   DatabaseTableModel* model);
  void deleteLastOrder(DatabaseTableModel* model);
  void showOrderTable(DatabaseTableModel* model);

  void createNewProductionLine(
      const QMap<QString, QString>* productionLineParameterseters,
      DatabaseTableModel* model);
  void allocateInactiveProductionLinesManually(const QString& orderId,
                                               DatabaseTableModel* model);
  void shutdownAllProductionLinesManually(DatabaseTableModel* model);
  void deleteLastProductionLine(DatabaseTableModel* model);
  void showProductionLineTable(DatabaseTableModel* model);
  void linkProductionLineWithBoxManually(
      const QMap<QString, QString>* linkParameterseters,
      DatabaseTableModel* model);

  void releaseTransponderManually(
      const QMap<QString, QString>* releaseParameters,
      TransponderSeedModel* seed);
  void confirmTransponderReleaseManually(
      const QMap<QString, QString>* confirmParameters,
      TransponderSeedModel* seed);
  void rereleaseTransponderManually(
      const QMap<QString, QString>* rereleaseParameters,
      TransponderSeedModel* seed);
  void confirmTransponderRereleaseManually(
      const QMap<QString, QString>* confirmParameters,
      TransponderSeedModel* seed);
  void searchTransponderManually(const QMap<QString, QString>* searchParameters,
                                 TransponderSeedModel* seed);
  void refundTransponderManually(const QMap<QString, QString>* refundParameters,
                                 TransponderSeedModel* seed);

  void initIssuers(DatabaseTableModel* model);
  void initTransportMasterKeys(DatabaseTableModel* model);
  void linkIssuerWithMasterKeys(DatabaseTableModel* model,
                                const QMap<QString, QString>* Parameterseters);

 private:
  void createAdministratorInstance(void);

  void createWaitingLoop(void);
  void createTimers(void);
  void setupODQTimer(uint32_t msecs);

  bool startOperationExecution(const QString& operationName);
  void endOperationExecution(const QString& operationName);

 private slots:
  void proxyLogging(const QString& log);

  void on_AdministratorBuilderCompleted_slot(void);
  void on_AdministratorThreadFinished_slot(void);

  void on_AdministratorFinished_slot(
      AdministrationSystem::ExecutionStatus status);
  void on_ODTimerTimeout_slot(void);
  void on_ODQTimerTimeout_slot(void);

 signals:
  void logging(const QString& log);
  void notifyUser(const QString& data);
  void notifyUserAboutError(const QString& data);
  void operationPerfomingStarted(void);
  void operationStepPerfomed(void);
  void operationPerformingFinished(void);
  void waitingEnd(void);

  void applySettings_signal(void);

  void authorize_signal(
      const std::shared_ptr<QMap<QString, QString>> authDataPtr);
  void authorizationSuccess_signal(
      const std::shared_ptr<QMap<QString, QString>> authDataPtr);
  void getDatabaseTable_signal(const QString& tableName,
                               DatabaseTableModel* model);
  void clearDatabaseTable_signal(const QString& tableName);
  void getCustomResponse_signal(const QString& req, DatabaseTableModel* model);

  void createNewOrder_signal(
      const QMap<QString, QString>* orderParameterseters);
  void startOrderAssembling_signal(const QString& orderId);
  void stopOrderAssembling_signal(const QString& orderId);
  void deleteLastOrder_signal(void);

  void createNewProductionLine_signal(
      const QMap<QString, QString>* productionLineParameterseters);
  void allocateInactiveProductionLines_signal(const QString& orderId);
  void shutdownAllProductionLines_signal(void);
  void removeLastProductionLine_signal(void);
  void linkProductionLineWithBox_signal(
      const QMap<QString, QString>* linkParameterseters);

  void initIssuerTable_signal(void);
  void initTransportMasterKeysTable_signal(void);
  void linkIssuerWithMasterKeys_signal(
      const QMap<QString, QString>* linkParameterseters);
};

//==================================================================================

#endif  // ADMIN_MANAGER_H
