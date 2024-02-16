#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include "abstract_manager.h"

class OrderManager : public AbstractManager
{
  Q_OBJECT
 public:
  explicit OrderManager(const QString& name);
  ~OrderManager();

  // AbstractManager interface
 public:
  virtual Type type() const override;

 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;

  // OrderManager interface
 public slots:
  void create(const std::shared_ptr<StringDictionary> param);
  void startAssembling(const std::shared_ptr<StringDictionary> param);
  void stopAssembling(const std::shared_ptr<StringDictionary> param);
  void get(const QString& name);

  void release(const std::shared_ptr<StringDictionary> param);
  void refund(const std::shared_ptr<StringDictionary> param);
  void generateShipmentRegister(const std::shared_ptr<StringDictionary> param);

  void initTransportMasterKeys(void);
  void initIssuers(void);
  void linkIssuerWithKeys(const std::shared_ptr<StringDictionary> param);
};

#endif // ORDERMANAGER_H
