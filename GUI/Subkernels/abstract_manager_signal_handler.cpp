#include "abstract_manager_signal_handler.h"

AbstractManagerSignalHandler::AbstractManagerSignalHandler(const QString& name)
    : AbstractGuiSubkernel{name} {
  doLoadSettings();
}

AbstractManagerSignalHandler::~AbstractManagerSignalHandler() {}

void AbstractManagerSignalHandler::processOperationStart(
    const QString& operationName) {}

void AbstractManagerSignalHandler::processOperationFinish(
    const QString& operationName,
    ReturnStatus ret) {}

void AbstractManagerSignalHandler::loadSettings() {
  doLoadSettings();
}

void AbstractManagerSignalHandler::doLoadSettings() {}

void AbstractManagerSignalHandler::createProgressDialog() {}

void AbstractManagerSignalHandler::destroyProgressDialog() {}

void AbstractManagerSignalHandler::createTimers() {}

void AbstractManagerSignalHandler::createMessageMatchTable() {}

void AbstractManagerSignalHandler::processReturnStatus(ReturnStatus ret) {}

void AbstractManagerSignalHandler::progressDialogCanceled_slot() {}

void AbstractManagerSignalHandler::ODTimerTimeout_slot() {}

void AbstractManagerSignalHandler::ODQTimerTimeout_slot() {}
