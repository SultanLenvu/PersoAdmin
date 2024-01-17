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
  bool LogEnable;
  QString ShipmentRegisterDir;

  PostgreSqlDatabase* Database;

 public:
  explicit AdministrationSystem(QObject* parent);
  void applySettings(void);

  ReturnStatus connectDatabase(void);
  ReturnStatus disconnectDatabase(void);

  ReturnStatus getTable(const QString& tableName, SqlQueryValues* response);
  ReturnStatus getCustomResponse(const QString& req, SqlQueryValues* response);

  ReturnStatus createNewOrder(
      const std::shared_ptr<QHash<QString, QString>> orderParameters);
  ReturnStatus startOrderAssembling(const QString& orderId);
  ReturnStatus stopOrderAssembling(const QString& orderId);

  ReturnStatus createNewProductionLine(
      const QHash<QString, QString>* productionLineParameters);
  ReturnStatus activateAllProductionLines(void) const;
  ReturnStatus deactivateAllProductionLines(void) const;
  ReturnStatus activateProductionLine(const QString& id);
  ReturnStatus deactivateProductionLine(const QString& id);
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

  ReturnStatus releaseTransponderManually(const QString& id);
  ReturnStatus releaseBoxManually(const QString& id);
  ReturnStatus releasePalletManually(const QString& id);
  ReturnStatus releaseOrderManually(const QString& id);

  ReturnStatus refundTransponderManually(const QString& id);
  ReturnStatus refundBoxManually(const QString& id);
  ReturnStatus refundPalletManually(const QString& id);
  ReturnStatus refundOrderManually(const QString& id);

  ReturnStatus shipPallets(const QHash<QString, QString>* param);

 private:
  Q_DISABLE_COPY_MOVE(AdministrationSystem) void createDatabase(void);
  void loadSettings(void);
  void sendLog(const QString& log) const;

  bool addOrder(
      const std::shared_ptr<QHash<QString, QString>> orderParameters) const;
  bool addPallets(
      const QString& orderId,
      const std::shared_ptr<QHash<QString, QString>> orderParameters) const;
  bool addBoxes(const QString& palletId,
                const std::shared_ptr<QHash<QString, QString>> orderParameters,
                QTextStream& panSource) const;
  bool addTransponders(
      const QString& boxId,
      const std::shared_ptr<QVector<QString>> pans,
      const std::shared_ptr<QHash<QString, QString>> orderParameters) const;
  bool addProductionLine(
      const QHash<QString, QString>* productionLineParameters) const;

  int32_t getLastId(const QString& table) const;
  bool stopAllProductionLines(void) const;

  bool startOrderProcessing(const QString& id) const;
  bool stopOrderProcessing(const QString& id) const;

  ReturnStatus shipPallet(const QString& id, QTextStream& registerOut);

 signals:
  void logging(const QString& log);
};

#endif  // ORDERCREATIONSYSTEM_H
