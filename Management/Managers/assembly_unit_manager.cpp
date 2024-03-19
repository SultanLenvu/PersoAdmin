#include "assembly_unit_manager.h"

AssemblyUnitManager::AssemblyUnitManager(const QString& name)
    : NamedObject(name) {
  connectDependecies();
}

ReturnStatus AssemblyUnitManager::logOn(const QString& param) {}

ReturnStatus AssemblyUnitManager::logOut() {}

ReturnStatus AssemblyUnitManager::requestBox() {}

ReturnStatus AssemblyUnitManager::refundCurrentBox() {}

ReturnStatus AssemblyUnitManager::completeCurrentBox() {}

ReturnStatus AssemblyUnitManager::releaseTransponder() {}

ReturnStatus AssemblyUnitManager::rereleaseTransponder(const QString& param) {}

ReturnStatus AssemblyUnitManager::rollbackTransponder() {}

void AssemblyUnitManager::connectDependecies() {}
