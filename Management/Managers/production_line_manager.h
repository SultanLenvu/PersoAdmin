#ifndef PRODUCTIONLINEMANAGER_H
#define PRODUCTIONLINEMANAGER_H

#include "abstract_manager.h"
#include "abstract_sql_database.h"

class ProductionLineManager final : public AbstractManager {
  Q_OBJECT
 private:
  std::shared_ptr<AbstractSqlDatabase> Database;

 public:
  explicit ProductionLineManager(const QString& name);
  ~ProductionLineManager();

  // Own
 public slots:
  void applyDatabase(std::shared_ptr<AbstractSqlDatabase> database);

  ReturnStatus create(const StringDictionary& param);

  ReturnStatus activate(const StringDictionary& param);
  ReturnStatus activateAll(void);

  ReturnStatus deactivate(const StringDictionary& param);
  ReturnStatus deactivateAll(void);

  ReturnStatus edit(const StringDictionary& param);
  ReturnStatus remove(const StringDictionary& param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineManager)
  void connectDependencies(void);

  bool addProductionLine(const StringDictionary& param);
  bool stopAllProductionLines(void);
};

#endif // PRODUCTIONLINEMANAGER_H
