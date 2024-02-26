#include "production_line_async_wrapper.h"

ProductionLineAsyncWrapper::ProductionLineAsyncWrapper(const QString& name) {}

ProductionLineAsyncWrapper::~ProductionLineAsyncWrapper() {}

void ProductionLineAsyncWrapper::onInstanceThreadStarted() {}

void ProductionLineAsyncWrapper::applyDatabase(
    std::shared_ptr<AbstractSqlDatabase> database) {}

void ProductionLineAsyncWrapper::create(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineAsyncWrapper::activate(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineAsyncWrapper::activateAll() {}

void ProductionLineAsyncWrapper::deactivate(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineAsyncWrapper::deactivateAll() {}

void ProductionLineAsyncWrapper::edit(
    const std::shared_ptr<StringDictionary> param) {}

void ProductionLineAsyncWrapper::remove(
    const std::shared_ptr<StringDictionary> param) {}
