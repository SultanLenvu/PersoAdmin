#ifndef ORDERCREATIONSYSTEM_H
#define ORDERCREATIONSYSTEM_H

#include <QApplication>
#include <QDate>
#include <QObject>

#include "Database/postgre_sql_database.h"

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
    ProductionLineMissed,
    ProductionLineLinkError,
    ProductionLineRollbackLimit,
    OrderRemovingError,
    OtherOrderInProcess,
    UnknownError,
    Completed
  };
  Q_ENUM(ReturnStatus)

 private:
  bool LogEnable;
  QString ShipmentRegisterDir;

  SqlQueryValues CurrentTransponder;
  SqlQueryValues CurrentBox;
  SqlQueryValues CurrentPallet;
  SqlQueryValues CurrentOrder;
  SqlQueryValues CurrentIssuer;

  PostgreSqlDatabase* Database;

 public:
  explicit AdministrationSystem(QObject* parent);
  void applySettings(void);

  ReturnStatus connectDatabase(void);
  ReturnStatus disconnectDatabase(void);

  ReturnStatus clearDatabaseTable(const QString& tableName);
  ReturnStatus getDatabaseTable(const QString& tableName,
                                SqlQueryValues* response);
  ReturnStatus getCustomResponse(const QString& req, SqlQueryValues* response);

  ReturnStatus createNewOrder(
      const QSharedPointer<QHash<QString, QString>> orderParameters);
  ReturnStatus startOrderAssembling(const QString& orderId) const;
  ReturnStatus stopOrderAssembling(const QString& orderId) const;
  ReturnStatus deleteLastOrder(void);

  ReturnStatus createNewProductionLine(
      const QHash<QString, QString>* productionLineParameters);
  ReturnStatus stopAllProductionLines(void) const;
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
      const QSharedPointer<QHash<QString, QString>> orderParameters) const;
  bool addPallets(
      const QString& orderId,
      const QSharedPointer<QHash<QString, QString>> orderParameters) const;
  bool addBoxes(
      const QString& palletId,
      const QSharedPointer<QHash<QString, QString>> orderParameters) const;
  bool addTransponders(
      const QString& boxId,
      const QSharedPointer<QVector<QString>> pans,
      const QSharedPointer<QHash<QString, QString>> orderParameters) const;
  bool addProductionLine(
      const QHash<QString, QString>* productionLineParameters) const;

  int32_t getLastId(const QString& table) const;

  bool startProductionLine(const QString& id, const QString& orderId) const;
  //  bool stopProductionLine(const QString& id) const;
  bool linkProductionLineWithBox(const QString& id, const QString& boxId) const;

  bool startBoxProcessing(const QString& id) const;
  bool startPalletProcessing(const QString& id) const;
  bool startOrderProcessing(const QString& id) const;

  //  bool removeLastProductionLine(void) const;
  //  bool stopBoxProcessing(const QString& id) const;
  //  bool stopPalletProcessing(const QString& id) const;
  //  bool stopOrderProcessing(const QString& id) const;

  bool searchFreeBox(const QString& orderId,
                     const QString& productionLineId,
                     SqlQueryValues& boxRecord) const;

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
