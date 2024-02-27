#include "programmer_async_wrapper.h"

ProgrammerAsyncWrapper::ProgrammerAsyncWrapper(const QString& name)
    : AbstractAsyncWrapper(name) {}

ProgrammerAsyncWrapper::~ProgrammerAsyncWrapper() {}

void ProgrammerAsyncWrapper::onInstanceThreadStarted() {}
