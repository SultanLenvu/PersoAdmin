#include "programmer_manager.h"

ProgrammerManager::ProgrammerManager(const QString& name)
    : AbstractManager(name) {}

ProgrammerManager::~ProgrammerManager() {}

void ProgrammerManager::onInstanceThreadStarted() {}

void ProgrammerManager::applySettings() {}
