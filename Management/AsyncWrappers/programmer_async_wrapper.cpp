#include "programmer_async_wrapper.h"

ProgrammerAsyncWrapper::ProgrammerAsyncWrapper(const QString& name)
    : NamedObject(name), LoggableObject(name) {}

ProgrammerAsyncWrapper::~ProgrammerAsyncWrapper() {}
