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

  // PSObject interface
 public:
  virtual void applySettings() override;

  // LogBackend interface
 public:
  virtual void writeLogMessage(const QString& str) override;

  // Own
 public:
  void clear();

 private:
  void loadSettings(void);

 signals:
  void displayLog_signal(const QString& log);
  void clearLogDisplay_signal(void);
};

#endif /* WIDGETLOGBACKEND_H */
