#include "assembly_unit_manager_async_wrapper.h"

AssemblyUnitManagerAsyncWrapper::AssemblyUnitManagerAsyncWrapper(
    const QString& name)
    : ProgressableAsyncWrapper(name) {}

void AssemblyUnitManagerAsyncWrapper::logOn(const StringDictionary& param) {
  initOperation("logOn");

  ReturnStatus ret;
  ret = Manager->logOn(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOn", ret);
    return;
  }

  completeOperation("logOn");
}

void AssemblyUnitManagerAsyncWrapper::logOut() {
  initOperation("logOut");

  ReturnStatus ret = Manager->logOut();
  if (ret != ReturnStatus::NoError) {
    processOperationError("logOut", ret);
    return;
  }

  completeOperation("logOut");
}

void AssemblyUnitManagerAsyncWrapper::requestBox() {
  initOperation("requestBox");

  ReturnStatus ret = Manager->requestBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("requestBox", ret);
    return;
  }

  completeOperation("requestBox");
}

void AssemblyUnitManagerAsyncWrapper::refundCurrentBox() {
  initOperation("refundCurrentBox");

  ReturnStatus ret = Manager->refundCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("refundCurrentBox", ret);
    return;
  }

  completeOperation("refundCurrentBox");
}

void AssemblyUnitManagerAsyncWrapper::completeCurrentBox() {
  initOperation("completeCurrentBox");

  ReturnStatus ret = Manager->completeCurrentBox();
  if (ret != ReturnStatus::NoError) {
    processOperationError("completeCurrentBox", ret);
    return;
  }

  completeOperation("completeCurrentBox");
}

void AssemblyUnitManagerAsyncWrapper::releaseTransponder() {
  initOperation("releaseTransponder");

  ReturnStatus ret = Manager->releaseTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("releaseTransponder", ret);
    return;
  }

  completeOperation("releaseTransponder");
}

void AssemblyUnitManagerAsyncWrapper::rereleaseTransponder(
    const StringDictionary& param) {
  initOperation("rereleaseTransponder");

  ReturnStatus ret = Manager->rereleaseTransponder(param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("rereleaseTransponder", ret);
    return;
  }

  completeOperation("rereleaseTransponder");
}

void AssemblyUnitManagerAsyncWrapper::rollbackTransponder() {
  initOperation("rollbackTransponder");

  ReturnStatus ret = Manager->rollbackTransponder();
  if (ret != ReturnStatus::NoError) {
    processOperationError("rollbackTransponder", ret);
    return;
  }

  completeOperation("rollbackTransponder");
}
