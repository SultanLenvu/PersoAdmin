#include "production_line_manager.h"

ProductionLineManager::ProductionLineManager(const QString& name)
    : AbstractManager(name) {}

ProductionLineManager::~ProductionLineManager() {}

AbstractManager::Type ProductionLineManager::type() const {
  return ProductionLine;
}

void ProductionLineManager::onInstanceThreadStarted() {}

void ProductionLineManager::applySettings() {}

void ProductionLineManager::create(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineManager::activate(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineManager::activateAll() {}

void ProductionLineManager::deactivate(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineManager::deactivateAll() {}

void ProductionLineManager::edit(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineManager::get(const QString& name) {}
