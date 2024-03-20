#ifndef ORDERMANAGERASYNCWRAPPER_H
#define ORDERMANAGERASYNCWRAPPER_H

#include "loggable_object.h"
#include "order_manager.h"
#include "progressable_async_wrapper.h"

class OrderManagerAsyncWrapper final : public ProgressableAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<OrderManager> Manager;

 public:
  Q_INVOKABLE OrderManagerAsyncWrapper(
      const QString& name,
      std::shared_ptr<ISqlDatabase> database);
  ~OrderManagerAsyncWrapper();

  // Own
 public slots:
  void create(const StringDictionary& param);
  void startAssembling(const StringDictionary& param);
  void stopAssembling(const StringDictionary& param);
  void generateShipmentRegister(const StringDictionary& param);

  void release(const StringDictionary& param);
  void refund(const StringDictionary& param);

  void initTransportMasterKeys(void);
  void initIssuers(void);
  void linkIssuerWithKeys(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(OrderManagerAsyncWrapper)
};

#endif // ORDERMANAGERASYNCWRAPPER_H
