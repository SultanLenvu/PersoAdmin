#ifndef PRODUCTIONLINEASYNCWRAPPER_H
#define PRODUCTIONLINEASYNCWRAPPER_H

#include "abstract_async_wrapper.h"
#include "loggable_object.h"
#include "named_object.h"
#include "production_line_manager.h"
#include "progressable_async_wrapper.h"

class ProductionLineManagerAsyncWrapper final : public NamedObject,
                                                public ProgressableAsyncWrapper,
                                                public LoggableObject {
  Q_OBJECT
 private:
  std::unique_ptr<ProductionLineManager> Manager;

 public:
  Q_INVOKABLE explicit ProductionLineManagerAsyncWrapper(
      const QString& name,
      std::shared_ptr<AbstractSqlDatabase> database);
  ~ProductionLineManagerAsyncWrapper();

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
