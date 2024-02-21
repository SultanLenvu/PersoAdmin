#ifndef DATABASEGUISUBKERNEL_H
#define DATABASEGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"
#include "sql_query_values.h"
#include "sql_response_model.h"

class DatabaseGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 private:
  std::unique_ptr<SqlResponseModel> ResponseModel;

 public:
  explicit DatabaseGuiSubkernel(const QString& name);
  ~DatabaseGuiSubkernel();

 public:
  SqlResponseModel* responseModel(void);

  // Слоты для GUI
 public slots:
  void connect(void);
  void disconnect(void);
  void getTable(const QString& name);
  void execCustomRequest(void);

  // Слоты для менеджеров
 public slots:
  void display(std::shared_ptr<SqlQueryValues> response);

 private:
  void connectDependecies(void);

 signals:
  void connect_signal(void);
  void disconnect_signal(void);
  void getTable_signal(const QString& name);
  void execCustomRequest_signal(const QString& req);
};

#endif // DATABASEGUISUBKERNEL_H
