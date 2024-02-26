#ifndef PRODUCTIONLINEASYNCWRAPPER_H
#define PRODUCTIONLINEASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "production_line_manager.h"

class ProductionLineAsyncWrapper final : public AbstractAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<ProductionLineManager> Manager;

 public:
  ProductionLineAsyncWrapper(const QString& name);
  ~ProductionLineAsyncWrapper();

  // AbstractAsyncWrapper interface
 public slots:
  virtual void onInstanceThreadStarted() override;

  // Own
 public slots:
  void applyDatabase(std::shared_ptr<AbstractSqlDatabase> database);

  void create(const std::shared_ptr<StringDictionary> param);

  void activate(const std::shared_ptr<StringDictionary> param);
  void activateAll(void);

  void deactivate(const std::shared_ptr<StringDictionary> param);
  void deactivateAll(void);

  void edit(const std::shared_ptr<StringDictionary> param);
  void remove(const std::shared_ptr<StringDictionary> param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineAsyncWrapper)
};

#endif // PRODUCTIONLINEASYNCWRAPPER_H
