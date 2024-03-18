#include "progress_indicator_link.h"
#include "global_environment.h"
#include "progress_indicator.h"

ProgressIndicatorLink::ProgressIndicatorLink() {
  ProgressIndicator* is = static_cast<ProgressIndicator*>(
      GlobalEnvironment::instance()->getObject("ProgressIndicator"));

  connect(this, &ProgressIndicatorLink::begin, is, &ProgressIndicator::begin);
  connect(this, &ProgressIndicatorLink::finish, is, &ProgressIndicator::finish);
}
