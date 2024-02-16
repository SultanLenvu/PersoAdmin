#include "database_manager.h"

DatabaseManager::DatabaseManager(const QString& name) : AbstractManager(name) {}

DatabaseManager::~DatabaseManager() {}

AbstractManager::Type DatabaseManager::type() const {
  return Database;
}

void DatabaseManager::onInstanceThreadStarted() {}

void DatabaseManager::applySettings() {}

void DatabaseManager::connect() {}

void DatabaseManager::disconnect() {}

void DatabaseManager::getTable(const QString& name) {}

void DatabaseManager::execCustomRequest(const QString& req) {}
