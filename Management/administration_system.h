#ifndef ORDERCREATIONSYSTEM_H
#define ORDERCREATIONSYSTEM_H

#include <QApplication>
#include <QDate>
#include <QObject>

#include "postgre_sql_database.h"
#include "types.h"

class AdministrationSystem : public QObject {
  Q_OBJECT
 private:
  QString ShipmentRegisterDir;

  PostgreSqlDatabase* Database;

 public:
  AdministrationSystem(const QString& name);
  void applySettings(void);

  ReturnStatus connectDatabase(void);
  ReturnStatus disconnectDatabase(void);

  ReturnStatus getTable(const QString& tableName, SqlQueryValues* response);
  ReturnStatus getCustomResponse(const QString& req, SqlQueryValues* response);

  ReturnStatus createNewOrder(const std::shared_ptr<StringDictionary> param);
  ReturnStatus startOrderAssembling(const QString& id);
  ReturnStatus stopOrderAssembling(const QString& id);

  ReturnStatus createNewProductionLine(const StringDictionary* param);
  ReturnStatus activateAllProductionLines(void) const;
  ReturnStatus deactivateAllProductionLines(void) const;
  ReturnStatus activateProductionLine(const QString& id);
  ReturnStatus deactivateProductionLine(const QString& id);
  ReturnStatus deleteLastProductionLine(void);

  ReturnStatus initIssuerTable(void);
  ReturnStatus initTransportMasterKeysTable(void);
  ReturnStatus linkIssuerWithMasterKeys(const StringDictionary* linkParameters);

  ReturnStatus getTransponderData(const QString& id, StringDictionary* data);
  ReturnStatus getBoxData(const QString& id, StringDictionary* data);
  ReturnStatus getPalletData(const QString& id, StringDictionary* data);

  ReturnStatus rollbackProductionLine(const QString& id);

  ReturnStatus releaseTransponder(const QString& id);
  ReturnStatus releaseBox(const QString& id);
  ReturnStatus releasePallet(const QString& id);
  ReturnStatus releaseOrder(const QString& id);

  ReturnStatus refundTransponder(const QString& id);
  ReturnStatus refundBox(const QString& id);
  ReturnStatus refundPallet(const QString& id);
  ReturnStatus refundOrder(const QString& id);

  ReturnStatus shipPallets(const StringDictionary* param);

 private:
  Q_DISABLE_COPY_MOVE(AdministrationSystem)
  void loadSettings(void);
  void sendLog(const QString& log) const;

  bool addOrder(const std::shared_ptr<StringDictionary> orderParameters) const;
  bool addPallets(
      const QString& orderId,
      const std::shared_ptr<StringDictionary> orderParameters) const;
  bool addBoxes(const QString& palletId,
                const std::shared_ptr<StringDictionary> orderParameters,
                QTextStream& panSource) const;
  bool addTransponders(
      const QString& boxId,
      const std::shared_ptr<QVector<QString>> pans,
      const std::shared_ptr<StringDictionary> orderParameters) const;
  bool addProductionLine(
      const StringDictionary* productionLineParameters) const;

  int32_t getLastId(const QString& table) const;
  bool stopAllProductionLines(void) const;

  bool startOrderProcessing(const QString& id) const;
  bool stopOrderProcessing(const QString& id) const;

  ReturnStatus shipPallet(const QString& id, QTextStream& registerOut);

  void createDatabase(void);

 signals:
  void logging(const QString& log);
};

#endif  // ORDERCREATIONSYSTEM_H
