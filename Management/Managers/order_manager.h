#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include "abstract_manager.h"
#include "abstract_sql_database.h"

class OrderManager : public AbstractManager
{
  Q_OBJECT
 private:
  QString ShipmentRegisterDir;
  std::shared_ptr<AbstractSqlDatabase> Database;

 public:
  explicit OrderManager(const QString& name);
  ~OrderManager();

  // Own
 public slots:
  void applyDatabase(std::shared_ptr<AbstractSqlDatabase> database);

  ReturnStatus create(const StringDictionary& param);
  ReturnStatus startAssembling(const StringDictionary& param);
  ReturnStatus stopAssembling(const StringDictionary& param);
  ReturnStatus generateShipmentRegister(const StringDictionary& param);

  ReturnStatus release(const StringDictionary& param);
  ReturnStatus refund(const StringDictionary& param);

  ReturnStatus initTransportMasterKeys(void);
  ReturnStatus initIssuers(void);
  ReturnStatus linkIssuerWithKeys(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(OrderManager)
  void connectDependencies(void);
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

  bool addOrder(const StringDictionary& param);
  bool addPallets(const QString& orderId, const StringDictionary& param);
  bool addBoxes(const QString& palletId,
                const StringDictionary& param,
                QTextStream& panSource);
  bool addTransponders(const QString& boxId,
                       const std::shared_ptr<QVector<QString>>& pans,
                       const StringDictionary& param);

  ReturnStatus releaseTransponder(const QString& id);
  ReturnStatus releaseBox(const QString& id);
  ReturnStatus releasePallet(const QString& id);
  ReturnStatus releaseOrder(const QString& id);

  ReturnStatus refundTransponder(const QString& id);
  ReturnStatus refundBox(const QString& id);
  ReturnStatus refundPallet(const QString& id);
  ReturnStatus refundOrder(const QString& id);

  ReturnStatus shipPallet(const QString& id, QTextStream& registerOut);
};

#endif // ORDERMANAGER_H
