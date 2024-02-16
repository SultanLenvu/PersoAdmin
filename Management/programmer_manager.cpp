#include "programmer_manager.h"

ProgrammerManager::ProgrammerManager(const QString& name)
    : AbstractManager(name) {}

ProgrammerManager::~ProgrammerManager() {}

AbstractManager::Type ProgrammerManager::type() const {
  return Programmer;
}

void ProgrammerManager::onInstanceThreadStarted() {}

void ProgrammerManager::applySettings() {}
