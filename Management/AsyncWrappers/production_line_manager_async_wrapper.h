#ifndef PRODUCTIONLINEASYNCWRAPPER_H
#define PRODUCTIONLINEASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "production_line_manager.h"

class ProductionLineManagerAsyncWrapper final : public AbstractAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<ProductionLineManager> Manager;

 public:
  ProductionLineManagerAsyncWrapper(const QString& name);
  ~ProductionLineManagerAsyncWrapper();

  // AbstractAsyncWrapper interface
 public slots:
  virtual void onInstanceThreadStarted() override;

  // Own
 public slots:
  void create(const std::shared_ptr<StringDictionary> param);

  void activate(const std::shared_ptr<StringDictionary> param);
  void activateAll(void);

  void deactivate(const std::shared_ptr<StringDictionary> param);
  void deactivateAll(void);

  void edit(const std::shared_ptr<StringDictionary> param);
  void remove(const std::shared_ptr<StringDictionary> param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineManagerAsyncWrapper)
};

#endif // PRODUCTIONLINEASYNCWRAPPER_H