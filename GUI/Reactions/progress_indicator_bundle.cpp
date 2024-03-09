#include "progress_indicator_bundle.h"
#include "global_environment.h"
#include "progress_indicator.h"

ProgressIndicatorBundle::ProgressIndicatorBundle() {
  ProgressIndicator* is = static_cast<ProgressIndicator*>(
      GlobalEnvironment::instance()->getObject("ProgressIndicator"));

  connect(this, &ProgressIndicatorBundle::executionStarted, is,
          &ProgressIndicator::begin);
  connect(this, &ProgressIndicatorBundle::executionFinished, is,
          &ProgressIndicator::finish);
}

void ProgressIndicatorBundle::begin(const QString& name) {
  emit executionStarted(name);
}

void ProgressIndicatorBundle::finish(const QString& name) {
  emit executionFinished(name);
}
