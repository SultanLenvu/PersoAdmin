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

  void create(const std::shared_ptr<StringDictionary> param);

  void activate(const std::shared_ptr<StringDictionary> param);
  void activateAll(void);

  void deactivate(const std::shared_ptr<StringDictionary> param);
  void deactivateAll(void);

  void edit(const std::shared_ptr<StringDictionary> param);
  void remove(const std::shared_ptr<StringDictionary> param);

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineManager)
  void loadSettings(void);
  void connectDependencies(void);

  bool addProductionLine(const StringDictionary& param);
  bool stopAllProductionLines(void);
};

#endif // PRODUCTIONLINEMANAGER_H
