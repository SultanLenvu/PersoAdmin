#include "widget_log_backend.h"
#include "global_environment.h"

WidgetLogBackend::WidgetLogBackend(const QString& name) : LogBackend(name) {
  loadSettings();

  GlobalEnvironment::instance()->registerObject(this);
}

void WidgetLogBackend::writeLogLine(const QString& str) {
  if (Enable) {
    emit displayLog_signal(str);
  }
}

void WidgetLogBackend::clear() {
  if (Enable) {
    emit clearLogDisplay_signal();
  }
}

void WidgetLogBackend::applySettings() {
  loadSettings();
}

void WidgetLogBackend::loadSettings() {
  QSettings settings;

  Enable = settings.value("log_system/display_log_enable").toBool();
}
