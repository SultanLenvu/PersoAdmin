#include <QSettings>

#include "widget_log_backend.h"

WidgetLogBackend::WidgetLogBackend(const QString& name) : LogBackend(name) {
  loadSettings();
}

WidgetLogBackend::~WidgetLogBackend() {}

void WidgetLogBackend::writeLogMessage(const QString& str) {
  if (Enable) {
    emit displayLog_signal(str);
  }
}

void WidgetLogBackend::applySettings() {
  loadSettings();
}

void WidgetLogBackend::clear() {
  if (Enable) {
    emit clearLogDisplay_signal();
  }
}

void WidgetLogBackend::loadSettings() {
  QSettings settings;

  Enable = settings.value("log_system/display_log_enable").toBool();
}
