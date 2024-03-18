#ifndef ORDERGUISUBKERNEL_H
#define ORDERGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"
#include "sql_response_model.h"

class OrderGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 private:
  SqlResponseModel Orders;

 public:
  explicit OrderGuiSubkernel(const QString& name);
  ~OrderGuiSubkernel() = default;

 public:
  SqlResponseModel& orders(void);

  // Слоты для сигналов от GUI
 public slots:
  void create(void);
  void startAssembling(void);
  void stopAssembling(void);
  void get(void);

  void release(void);
  void refund(void);
  void generateShipmentRegister(void);

  void initTransportMasterKeys(void);
  void initIssuers(void);
  void linkIssuerWithKeys(void);

  // Слоты для сигналов от менеджеров
 public slots:
  void displayResponse(std::shared_ptr<SqlQueryValues> orders);

 private:
  void connectDependecies(void);

 signals:
  void create_signal(const std::shared_ptr<StringDictionary> param);
  void startAssembling_signal(const std::shared_ptr<StringDictionary> param);
  void stopAssembling_signal(const std::shared_ptr<StringDictionary> param);
  void get_signal(const QString& name);

  void release_signal(const std::shared_ptr<StringDictionary> param);
  void refund_signal(const std::shared_ptr<StringDictionary> param);
  void shipPallets_signal(const std::shared_ptr<StringDictionary> param);

  void initTransportMasterKeys_signal(void);
  void initIssuers_signal(void);
  void linkIssuerWithKeys_signal(const std::shared_ptr<StringDictionary> param);
};

#endif // ORDERGUISUBKERNEL_H
