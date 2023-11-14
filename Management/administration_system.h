#ifndef ORDERCREATIONSYSTEM_H
#define ORDERCREATIONSYSTEM_H

#include <QApplication>
#include <QDate>
#include <QObject>

#include "Database/postgre_sql_database.h"
#include "Database/sql_response_model.h"

class AdministrationSystem : public QObject {
  Q_OBJECT
 public:
  enum ReturnStatus {
    NotExecuted,
    ParameterError,
    DatabaseConnectionError,
    DatabaseTransactionError,
    DatabaseQueryError,
    ShipmentRegisterError,
    FreeProductionLineMissed,
    LogicError,
    UnknownError,
    Completed
  };
  Q_ENUM(ReturnStatus)

 private:
  bool LogEnable;
  QString ShipmentRegisterDir;

  SqlResponseModel CurrentTransponder;
  SqlResponseModel CurrentBox;
  SqlResponseModel CurrentPallet;
  SqlResponseModel CurrentOrder;
  SqlResponseModel CurrentIssuer;

  PostgreSqlDatabase* Database;

 public:
  explicit AdministrationSystem(QObject* parent);
  void applySettings(void);

  ReturnStatus connectDatabase(void);
  ReturnStatus disconnectDatabase(void);

  ReturnStatus clearDatabaseTable(const QString& tableName);
  ReturnStatus getDatabaseTable(const QString& tableName,
                                SqlResponseModel* response);
  ReturnStatus getCustomResponse(const QString& req,
                                 SqlResponseModel* response);

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

  ReturnStatus rollbackProductionLine(const QString& id);

  ReturnStatus releaseTranspondersManually(const QString& table,
                                           const QString& id);
  ReturnStatus refundTranspondersManually(const QString& table,
                                          const QString& id);
  ReturnStatus shipPallets(const QHash<QString, QString>* param);

 private:
  Q_DISABLE_COPY(AdministrationSystem) void createDatabase(void);
  void loadSettings(void);
  void sendLog(const QString& log) const;

  bool getCurrentContext(const QString& id);

  bool addOrder(
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addPallets(
      const QString& orderId,
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addBoxes(
      const QString& orderId,
      const QSharedPointer<QHash<QString, QString> > orderParameters) const;
  bool addTransponders(
      const QString& orderId,
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
                                  SqlResponseModel& boxRecord) const;

  ReturnStatus releaseTransponderManually(const QString& id);
  ReturnStatus releaseBoxManually(const QString& id);
  ReturnStatus releasePalletManually(const QString& id);
  ReturnStatus releaseOrderManually(const QString& id);

  ReturnStatus refundTransponderManually(const QString& id);
  ReturnStatus refundBoxManually(const QString& id);
  ReturnStatus refundPalletManually(const QString& id);
  ReturnStatus refundOrderManually(const QString& id);

  ReturnStatus shipTransponder(const QString& id, QFile* reg);
  ReturnStatus shipPallet(const QString& id, QFile* reg);

 signals:
  void logging(const QString& log) const;
};

#endif  // ORDERCREATIONSYSTEM_H
