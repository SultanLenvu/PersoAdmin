#include "production_line_manager_async_wrapper.h"

ProductionLineManagerAsyncWrapper::ProductionLineManagerAsyncWrapper(
    const QString& name,
    std::shared_ptr<ISqlDatabase> database)
    : ProgressableAsyncWrapper{name},
      Manager(new ProductionLineManager("ProductionLineManager", database)) {}

ProductionLineManagerAsyncWrapper::~ProductionLineManagerAsyncWrapper() {}

void ProductionLineManagerAsyncWrapper::create(
    const StringDictionary& param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("create");

  ret = Manager->create(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("create", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("create");
}

void ProductionLineManagerAsyncWrapper::activate(
    const StringDictionary& param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("activate");

  ret = Manager->activate(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("activate", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("activate");
}

void ProductionLineManagerAsyncWrapper::activateAll() {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("activateAll");

  ret = Manager->activateAll();
  if (ret != ReturnStatus::NoError) {
    processOperationError("activateAll", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("activateAll");
}

void ProductionLineManagerAsyncWrapper::deactivate(
    const StringDictionary& param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("deactivate");

  ret = Manager->deactivate(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("deactivate", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("deactivate");
}

void ProductionLineManagerAsyncWrapper::deactivateAll() {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("deactivateAll");

  ret = Manager->deactivateAll();
  if (ret != ReturnStatus::NoError) {
    processOperationError("deactivateAll", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("deactivateAll");
}

void ProductionLineManagerAsyncWrapper::edit(
    const StringDictionary& param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("edit");

  ret = Manager->deactivate(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("edit", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("edit");
}

void ProductionLineManagerAsyncWrapper::remove(
    const StringDictionary& param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("remove");

  ret = Manager->deactivate(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("remove", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("remove");
}
