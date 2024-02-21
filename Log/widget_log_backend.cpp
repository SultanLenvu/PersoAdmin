#include <QSettings>

#include "widget_log_backend.h"

WidgetLogBackend::WidgetLogBackend(const QString& name) : LogBackend(name) {
  doLoadSettings();
}

WidgetLogBackend::~WidgetLogBackend() {}

void WidgetLogBackend::writeLogMessage(const QString& str) {
  if (Enable) {
    emit displayLog_signal(str);
  }
}

void WidgetLogBackend::clear() {
  if (Enable) {
    emit clearLogDisplay_signal();
  }
}

void WidgetLogBackend::loadSettings() {
  doLoadSettings();
}

void WidgetLogBackend::doLoadSettings() {
  QSettings settings;

  Enable = settings.value("log_system/display_log_enable").toBool();
}
