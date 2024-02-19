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
  virtual void applySettings() override;

  // OrderManager interface
 public:
  void setDatabase(std::shared_ptr<AbstractSqlDatabase> database);

  // Own
 public slots:
  void create(const std::shared_ptr<StringDictionary> param);
  void startAssembling(const std::shared_ptr<StringDictionary> param);
  void stopAssembling(const std::shared_ptr<StringDictionary> param);

  void release(const std::shared_ptr<StringDictionary> param);
  void refund(const std::shared_ptr<StringDictionary> param);
  void generateShipmentRegister(const std::shared_ptr<StringDictionary> param);

  void initTransportMasterKeys(void);
  void initIssuers(void);
  void linkIssuerWithKeys(const std::shared_ptr<StringDictionary> param);

 private:
  bool addOrder(const StringDictionary& param);
  bool addPallets(const QString& orderId, const StringDictionary& param);
  bool addBoxes(const QString& palletId,
                const StringDictionary& param,
                QTextStream& panSource);
  bool addTransponders(const QString& boxId,
                       const std::shared_ptr<QVector<QString>>& pans,
                       const StringDictionary& param);

  bool stopAllProductionLines(void);

  bool releaseTransponder(const QString& id);
  bool releaseBox(const QString& id);
  bool releasePallet(const QString& id);
  bool releaseOrder(const QString& id);

  bool refundTransponder(const QString& id);
  bool refundBox(const QString& id);
  bool refundPallet(const QString& id);
  bool refundOrder(const QString& id);

  int32_t getLastId(const QString& table);
};

#endif // ORDERMANAGER_H
