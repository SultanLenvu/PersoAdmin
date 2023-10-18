#ifndef ORDERCREATIONSYSTEM_H
#define ORDERCREATIONSYSTEM_H

#include <QApplication>
#include <QDate>
#include <QObject>

#include "Database/database_controller.h"
#include "Database/database_table_model.h"
#include "Database/postgres_controller.h"
#include "Log/log_system.h"
#include "Transponder/transponder_seed_model.h"

class AdministrationSystem : public QObject {
  Q_OBJECT
 public:
  enum ReturnStatus {
    NotExecuted,
    DatabaseConnectionError,
    DatabaseTransactionError,
    DatabaseQueryError,
    LogicError,
    UnknownError,
    Completed
  };
  Q_ENUM(ReturnStatus)

 private:
  bool LogEnable;

  PostgresController* Database;

 public:
  explicit AdministrationSystem(QObject* parent);
  void applySettings(void);

  ReturnStatus connectDatabase(void);
  ReturnStatus disconnectDatabase(void);

  ReturnStatus clearDatabaseTable(const QString& tableName);
  ReturnStatus getDatabaseTable(const QString& tableName,
                                DatabaseTableModel* buffer);
  ReturnStatus getCustomResponse(const QString& req,
                                 DatabaseTableModel* buffer);

  ReturnStatus createNewOrder(
      const QSharedPointer<QHash<QString, QString> > orderParameters);
  ReturnStatus startOrderAssembling(const QString& orderId);
  ReturnStatus stopOrderAssembling(const QString& orderId);
  ReturnStatus deleteLastOrder(void);

  ReturnStatus createNewProductionLine(
      const QHash<QString, QString>* productionLineParameters);
  ReturnStatus allocateInactiveProductionLines(const QString& orderId);
  ReturnStatus linkProductionLineWithBox(
      const QHash<QString, QString>* linkParameters);
  ReturnStatus shutdownAllProductionLines(void);
  ReturnStatus deleteLastProductionLine(void);

  ReturnStatus initIssuerTable(void);
  ReturnStatus initTransportMasterKeysTable(void);
  ReturnStatus linkIssuerWithMasterKeys(
      const QHash<QString, QString>* linkParameters);

  ReturnStatus getTransponderData(const QString& id,
                                  QHash<QString, QString>* data);
  ReturnStatus getBoxData(const QString& id, QHash<QString, QString>* data);
  ReturnStatus getPalletData(const QString& id, QHash<QString, QString>* data);

 private:
  Q_DISABLE_COPY(AdministrationSystem)
  void createDatabaseController(void);
  void loadSettings(void);
  void sendLog(const QString& log) const;

  bool addOrder(
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addPallets(
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addBoxes(
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addTransponders(
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addProductionLine(
      const QHash<QString, QString>* productionLineParameters) const;

  bool startBoxProcessing(const QString& id,
                          const QString& productionLineId) const;
  bool startPalletProcessing(const QString& id) const;
  bool startOrderProcessing(const QString& id) const;

  bool removeLastProductionLine(void) const;
  bool stopBoxProcessing(const QString& id) const;
  bool stopPalletProcessing(const QString& id) const;
  bool stopOrderProcessing(const QString& id) const;

  bool searchBoxForProductionLine(const QString& orderId,
                                  const QString& productionLineId,
                                  QHash<QString, QString>& boxRecord) const;

 signals:
  void logging(const QString& log) const;
};

#endif  // ORDERCREATIONSYSTEM_H
