#ifndef ORDERMANAGERASYNCWRAPPER_H
#define ORDERMANAGERASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "order_manager.h"

class OrderManagerAsyncWrapper final : public AbstractAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<OrderManager> Manager;

 public:
  OrderManagerAsyncWrapper(const QString& name,
                           std::shared_ptr<AbstractSqlDatabase> database);
  ~OrderManagerAsyncWrapper();

  // Own
 public slots:
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
  Q_DISABLE_COPY_MOVE(OrderManagerAsyncWrapper)
};

#endif // ORDERMANAGERASYNCWRAPPER_H
