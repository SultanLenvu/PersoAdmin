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

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;

  // Own
 public slots:
  void applyDatabase(std::shared_ptr<AbstractSqlDatabase> database);

  void create(const std::shared_ptr<StringDictionary> param);
  void startAssembling(const std::shared_ptr<StringDictionary> param);
  void stopAssembling(const std::shared_ptr<StringDictionary> param);
  void generateShipmentRegister(const std::shared_ptr<StringDictionary> param);

  void release(const std::shared_ptr<StringDictionary> param);
  void refund(const std::shared_ptr<StringDictionary> param);

  void initTransportMasterKeys(void);
  void initIssuers(void);
  void linkIssuerWithKeys(const std::shared_ptr<StringDictionary> param);

 private:
  Q_DISABLE_COPY_MOVE(OrderManager)
  void connectDependencies(void);

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
