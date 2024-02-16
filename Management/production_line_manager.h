#ifndef PRODUCTIONLINEMANAGER_H
#define PRODUCTIONLINEMANAGER_H

#include "abstract_manager.h"

class ProductionLineManager : public AbstractManager
{
  Q_OBJECT
 public:
  explicit ProductionLineManager(const QString& name);
  ~ProductionLineManager();

  // AbstractManager interface
 public:
  virtual Type type() const override;

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
  void get(const QString& name);
};

#endif // PRODUCTIONLINEMANAGER_H
