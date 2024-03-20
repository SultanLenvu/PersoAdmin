#include "abstract_gui_subkernel.h"
#include "global_environment.h"
#include "widget_log_backend.h"

AbstractGuiSubkernel::AbstractGuiSubkernel(const QString& name)
    : NamedObject{name} {
  connectDependecies();
}

void AbstractGuiSubkernel::connectDependecies() {
  WidgetLogBackend* wlb =
      GlobalEnvironment::instance()->getObject<WidgetLogBackend>(
          "WidgetLogBackend");

  connect(this, &AbstractGuiSubkernel::clearLogDisplay, wlb,
          &WidgetLogBackend::clear);
}
