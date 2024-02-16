#include "order_manager.h"

OrderManager::OrderManager(const QString& name) : AbstractManager(name) {}

OrderManager::~OrderManager() {}

AbstractManager::Type OrderManager::type() const {
  return Order;
}
void OrderManager::onInstanceThreadStarted() {}

void OrderManager::applySettings() {}

void OrderManager::create(const std::shared_ptr<StringDictionary> param) {}

void OrderManager::startAssembling(
    const std::shared_ptr<StringDictionary> param) {}

void OrderManager::stopAssembling(
    const std::shared_ptr<StringDictionary> param) {}

void OrderManager::get(const QString& name) {}

void OrderManager::release(const std::shared_ptr<StringDictionary> param) {}

void OrderManager::refund(const std::shared_ptr<StringDictionary> param) {}

void OrderManager::generateShipmentRegister(const std::shared_ptr<StringDictionary> param) {}

void OrderManager::initTransportMasterKeys() {}

void OrderManager::initIssuers() {}

void OrderManager::linkIssuerWithKeys(
    const std::shared_ptr<StringDictionary> param) {}
