#ifndef PRODUCTIONLINEMANAGER_H
#define PRODUCTIONLINEMANAGER_H

#include "abstract_manager.h"
#include "abstract_sql_database.h"

class ProductionLineManager : public AbstractManager {
  Q_OBJECT
 private:
  std::shared_ptr<AbstractSqlDatabase> Database;

 public:
  explicit ProductionLineManager(const QString& name,
                                 std::shared_ptr<AbstractSqlDatabase> database);
  ~ProductionLineManager();

  // AbstractManager interface
 public slots:
  virtual void onInstanceThreadStarted() override;
  virtual void applySettings() override;

  // Own
 public slots:
  void create(const std::shared_ptr<StringDictionary> param);

  void activate(const std::shared_ptr<StringDictionary> param);
  void activateAll(void);

  void deactivate(const std::shared_ptr<StringDictionary> param);
  void deactivateAll(void);

  void edit(const std::shared_ptr<StringDictionary> param);
  void remove(const std::shared_ptr<StringDictionary> param);
  void get(const QString& name);

 private:
  void loadSettings(void);

  bool addProductionLine(const StringDictionary& param);
  bool stopAllProductionLines(void);
};

#endif // PRODUCTIONLINEMANAGER_H
