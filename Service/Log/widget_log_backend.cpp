#include <QSettings>

#include "widget_log_backend.h"

WidgetLogBackend::WidgetLogBackend(const QString& name) : LogBackend(name) {
  doLoadSettings();
}

WidgetLogBackend::~WidgetLogBackend() {}

void WidgetLogBackend::writeMessage(const QString& str) {
  if (QApplication::instance()->thread() != thread()) {
    qDebug() << "Сервисное пространство запущено в отдельном потоке.";
  } else {
    qDebug() << "Сервисное пространство запущено в главном потоке.";
  }

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
