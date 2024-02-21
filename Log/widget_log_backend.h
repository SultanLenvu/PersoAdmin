#ifndef WIDGETLOGBACKEND_H
#define WIDGETLOGBACKEND_H

#include <QApplication>
#include <QObject>

#include "Log/log_backend.h"

class WidgetLogBackend final : public LogBackend {
  Q_OBJECT
 private:
  bool Enable;

 public:
  WidgetLogBackend(const QString& name);
  ~WidgetLogBackend();

  // LogBackend interface
 public:
  virtual void writeLogMessage(const QString& str) override;

  // Own
 public:
  void clear();

 private:
  Q_DISABLE_COPY_MOVE(WidgetLogBackend)
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

 signals:
  void displayLog_signal(const QString& log);
  void clearLogDisplay_signal(void);
};

#endif /* WIDGETLOGBACKEND_H */
