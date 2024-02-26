#include "order_manager_async_wrapper.h"

OrderManagerAsyncWrapper::OrderManagerAsyncWrapper(const QString& name)
    : AbstractAsyncWrapper{name} {}

OrderManagerAsyncWrapper::~OrderManagerAsyncWrapper() {}

void OrderManagerAsyncWrapper::onInstanceThreadStarted() {
  Manager = std::unique_ptr<OrderManager>(new OrderManager("OrderManager"));
}

void OrderManagerAsyncWrapper::create(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("create");

  ret = Manager->create(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("create", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("create");
}

void OrderManagerAsyncWrapper::startAssembling(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("startAssembling");

  ret = Manager->startAssembling(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("startAssembling", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("startAssembling");
}

void OrderManagerAsyncWrapper::stopAssembling(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("stopAssembling");

  ret = Manager->stopAssembling(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("stopAssembling", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("stopAssembling");
}

void OrderManagerAsyncWrapper::generateShipmentRegister(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("generateShipmentRegister");

  ret = Manager->generateShipmentRegister(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("generateShipmentRegister",
                          ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("generateShipmentRegister");
}

void OrderManagerAsyncWrapper::release(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("release");

  ret = Manager->release(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("release", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("release");
}

void OrderManagerAsyncWrapper::refund(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("refund");

  ret = Manager->refund(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("refund", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("refund");
}

void OrderManagerAsyncWrapper::initTransportMasterKeys() {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("initTransportMasterKeys");

  ret = Manager->initTransportMasterKeys();
  if (ret != ReturnStatus::NoError) {
    processOperationError("initTransportMasterKeys",
                          ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("initTransportMasterKeys");
}

void OrderManagerAsyncWrapper::initIssuers() {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("initIssuers");

  ret = Manager->initIssuers();
  if (ret != ReturnStatus::NoError) {
    processOperationError("initIssuers", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("initIssuers");
}

void OrderManagerAsyncWrapper::linkIssuerWithKeys(
    const std::shared_ptr<StringDictionary> param) {
  ReturnStatus ret = ReturnStatus::NoError;
  initOperation("linkIssuerWithKeys");

  ret = Manager->linkIssuerWithKeys(*param);
  if (ret != ReturnStatus::NoError) {
    processOperationError("linkIssuerWithKeys", ReturnStatus::DatabaseMissed);
    return;
  }

  completeOperation("linkIssuerWithKeys");
}
