#include "database_gui_subkernel.h"

#include "database_async_wrapper.h"
#include "global_environment.h"

DatabaseGuiSubkernel::DatabaseGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name), ResponseModel(new SqlResponseModel()) {
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

void DatabaseGuiSubkernel::displayResponse(
    std::shared_ptr<SqlQueryValues> response) {
  ResponseModel->setResponse(response);
}

void DatabaseGuiSubkernel::connectDependecies() {
  DatabaseAsyncWrapper* dm = static_cast<DatabaseAsyncWrapper*>(
      GlobalEnvironment::instance()->getObject("DatabaseAsyncWrapper"));

  // К менеджерам
  QObject::connect(this, &DatabaseGuiSubkernel::connect_signal, dm,
                   &DatabaseAsyncWrapper::connect);
  QObject::connect(this, &DatabaseGuiSubkernel::disconnect_signal, dm,
                   &DatabaseAsyncWrapper::disconnect);
  QObject::connect(this, &DatabaseGuiSubkernel::getTable_signal, dm,
                   &DatabaseAsyncWrapper::getTable);
  QObject::connect(this, &DatabaseGuiSubkernel::execCustomRequest_signal, dm,
                   &DatabaseAsyncWrapper::execCustomRequest);
}

void DatabaseGuiSubkernel::connectSignal_ResponseReady() {}
