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
#include "Transponder/transponder_seed_model.h"
#include "administration_system.h"

class AdminManager : public QObject {
  Q_OBJECT

 public:
  enum OperationState { Ready, WaitingExecution, Failed, Completed };
  Q_ENUM(OperationState)

 private:
  bool LogEnable;
  OperationState CurrentState;

  AdministrationSystem* Administrator;

 public:
  AdminManager(QObject* parent);
  ~AdminManager();

 public slots:
  void applySettings();
  void on_InsctanceThreadStarted(void);

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

 private:
  Q_DISABLE_COPY(AdminManager)
  void loadSettings(void);
  void sendLog(const QString& log) const;

  void createAdministrator(void);

  bool startOperationExecution(const QString& operationName);
  void finishOperationExecution(const QString& operationName,
                                const QString& msg);

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
