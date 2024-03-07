#ifndef WIDGETLOGBACKEND_H
#define WIDGETLOGBACKEND_H

#include <QApplication>
#include <QObject>

#include "abstract_log_backend.h"
#include "configurable_object.h"

class WidgetLogBackend final : public QObject,
                               public AbstractLogBackend,
                               public ConfigurableObject {
  Q_OBJECT
 private:
  bool Enable;

 public:
  explicit WidgetLogBackend();
  ~WidgetLogBackend();

  // AbstractLogBackend interface
 public:
  virtual void writeMessage(const QString& str) override;

  // Own
 public:
  void clear();

 private:
  Q_DISABLE_COPY_MOVE(WidgetLogBackend)

 public:
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

 signals:
  void displayLog_signal(const QString& log);
  void clearLogDisplay_signal(void);
};

#endif /* WIDGETLOGBACKEND_H */
