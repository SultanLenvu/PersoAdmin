#ifndef PRODUCTIONGUISUBKERNEL_H
#define PRODUCTIONGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"
#include "sql_response_model.h"

class ProductionLineGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 private:
  std::unique_ptr<SqlResponseModel> ProductionLines;

 public:
  explicit ProductionLineGuiSubkernel(const QString& name);
  ~ProductionLineGuiSubkernel();

 public:
  SqlResponseModel* productionLines(void);

 public slots:  // Слоты для сигналов от GUI
  void create(void);
  void activate(void);
  void activateAll(void);
  void deactivate(void);
  void deactivateAll(void);
  void edit(void);
  void get(void);

 public slots:  // Слоты для сигналов от менеджеров
  void displayResponse(std::shared_ptr<SqlQueryValues> data);

 private:
  void connectDependecies(void);

 signals:
  void create_signal(const std::shared_ptr<StringDictionary> param);
  void activate_signal(const std::shared_ptr<StringDictionary> param);
  void activateAll_signal(void);
  void deactivate_signal(const std::shared_ptr<StringDictionary> param);
  void deactivateAll_signal(void);
  void edit_signal(const std::shared_ptr<StringDictionary> param);
  void get_signal(const QString& name);
};

#endif // PRODUCTIONGUISUBKERNEL_H
