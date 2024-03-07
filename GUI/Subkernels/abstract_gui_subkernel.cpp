#include "abstract_gui_subkernel.h"
#include "global_environment.h"
#include "widget_log_backend.h"

AbstractGuiSubkernel::AbstractGuiSubkernel(const QString& name)
    : NamedObject{name} {
  connectDependecies();
}

AbstractGuiSubkernel::~AbstractGuiSubkernel() {}

void AbstractGuiSubkernel::connectDependecies() {
  WidgetLogBackend* wlb = static_cast<WidgetLogBackend*>(
      GlobalEnvironment::instance()->getObject("WidgetLogBackend"));

  connect(this, &AbstractGuiSubkernel::clearLogDisplay, wlb,
          &WidgetLogBackend::clear);
}
