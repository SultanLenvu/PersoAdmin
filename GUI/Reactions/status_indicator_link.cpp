#include "status_indicator_link.h"
#include "global_environment.h"
#include "status_indicator.h"

StatusIndicatorLink::StatusIndicatorLink() : QObject{nullptr} {
  StatusIndicator* si = static_cast<StatusIndicator*>(
      GlobalEnvironment::instance()->getObject("StatusIndicator"));

  connect(this, &StatusIndicatorLink::handle, si, &StatusIndicator::handle);
}
