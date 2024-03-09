#ifndef ORDERMANAGERASYNCWRAPPER_H
#define ORDERMANAGERASYNCWRAPPER_H

#include "loggable_object.h"
#include "named_object.h"
#include "order_manager.h"
#include "progressable_async_wrapper.h"

class OrderManagerAsyncWrapper final : public NamedObject,
                                       public ProgressableAsyncWrapper,
                                       public LoggableObject {
  Q_OBJECT
 private:
  std::unique_ptr<OrderManager> Manager;

 public:
  Q_INVOKABLE OrderManagerAsyncWrapper(
      const QString& name,
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
