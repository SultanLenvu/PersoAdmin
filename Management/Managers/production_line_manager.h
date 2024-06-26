#ifndef PRODUCTIONLINEMANAGER_H
#define PRODUCTIONLINEMANAGER_H

#include "i_manager.h"
#include "i_sql_database.h"
#include "loggable_object.h"
#include "named_object.h"

class ProductionLineManager final : public NamedObject,
                                    public IManager,
                                    public LoggableObject {
  Q_OBJECT
 private:
  std::shared_ptr<ISqlDatabase> Database;

 public:
  explicit ProductionLineManager(const QString& name,
                                 std::shared_ptr<ISqlDatabase> database);
  ~ProductionLineManager();

  // Own
 public:
  ReturnStatus create(const StringDictionary& param);

  ReturnStatus activate(const StringDictionary& param);
  ReturnStatus activateAll(void);

  ReturnStatus deactivate(const StringDictionary& param);
  ReturnStatus deactivateAll(void);

  ReturnStatus edit(const StringDictionary& param);
  ReturnStatus remove(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineManager)

 private:
  bool addProductionLine(const StringDictionary& param);
  bool stopAllProductionLines(void);
};

#endif // PRODUCTIONLINEMANAGER_H
