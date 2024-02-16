#include "database_gui_subkernel.h"

#include "database_manager.h"
#include "global_environment.h"

DatabaseGuiSubkernel::DatabaseGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();

  ResponseModel = std::unique_ptr<SqlResponseModel>(new SqlResponseModel());
}

DatabaseGuiSubkernel::~DatabaseGuiSubkernel() {}

SqlResponseModel* DatabaseGuiSubkernel::responseModel() {
  return ResponseModel.get();
}

void DatabaseGuiSubkernel::connect() {
  emit clearLogDisplay();

  emit connect_signal();
}

void DatabaseGuiSubkernel::disconnect() {
  emit clearLogDisplay();

  emit disconnect_signal();
}

void DatabaseGuiSubkernel::getTable(const QString& name) {
  emit clearLogDisplay();

  emit getTable_signal(name);
}

void DatabaseGuiSubkernel::execCustomRequest() {
  emit clearLogDisplay();
}

void DatabaseGuiSubkernel::displaySqlResponse(
    std::shared_ptr<SqlQueryValues> response) {
  ResponseModel->setResponse(response);
}

void DatabaseGuiSubkernel::connectDependecies() {
  DatabaseManager* dm = static_cast<DatabaseManager*>(
      GlobalEnvironment::instance()->getObject("DatabaseManager"));

  QObject::connect(this, &DatabaseGuiSubkernel::connect_signal, dm,
                   &DatabaseManager::connect);
  QObject::connect(this, &DatabaseGuiSubkernel::disconnect_signal, dm,
                   &DatabaseManager::disconnect);
  QObject::connect(this, &DatabaseGuiSubkernel::getTable_signal, dm,
                   &DatabaseManager::getTable);
  QObject::connect(this, &DatabaseGuiSubkernel::execCustomRequest_signal, dm,
                   &DatabaseManager::execCustomRequest);
}
