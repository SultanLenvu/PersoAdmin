#ifndef PRODUCTIONLINEASYNCWRAPPER_H
#define PRODUCTIONLINEASYNCWRAPPER_H

#include "i_async_wrapper.h"
#include "production_line_manager.h"
#include "progressable_async_wrapper.h"

class ProductionLineManagerAsyncWrapper final
    : public ProgressableAsyncWrapper {
  Q_OBJECT
 private:
  std::unique_ptr<ProductionLineManager> Manager;

 public:
  Q_INVOKABLE explicit ProductionLineManagerAsyncWrapper(
      const QString& name,
      std::shared_ptr<ISqlDatabase> database);
  ~ProductionLineManagerAsyncWrapper();

  // Own
 public slots:
  void create(const StringDictionary& param);

  void activate(const StringDictionary& param);
  void activateAll(void);

  void deactivate(const StringDictionary& param);
  void deactivateAll(void);

  void edit(const StringDictionary& param);
  void remove(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineManagerAsyncWrapper)
};

#endif // PRODUCTIONLINEASYNCWRAPPER_H
