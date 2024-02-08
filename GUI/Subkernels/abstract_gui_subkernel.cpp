#include "abstract_gui_subkernel.h"
#include "global_environment.h"
#include "widget_log_backend.h"

AbstractGuiSubkernel::AbstractGuiSubkernel(const QString& name)
    : PSObject{name} {
  connectDependecies();
}

AbstractGuiSubkernel::~AbstractGuiSubkernel() {}

void AbstractGuiSubkernel::connectDependecies() {
  connect(this, &AbstractGuiSubkernel::clearLogDisplay_signal,
          dynamic_cast<WidgetLogBackend*>(
              GlobalEnvironment::instance()->getObject("FileLogBackend")),
          &WidgetLogBackend::clear);
}
