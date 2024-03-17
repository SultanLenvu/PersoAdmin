#ifndef WIDGETLOGBACKEND_H
#define WIDGETLOGBACKEND_H

#include "abstract_log_backend.h"
#include "configurable_object.h"
#include "loggable_object.h"
#include "named_object.h"

class WidgetLogBackend final : public NamedObject,
                               public AbstractLogBackend,
                               public ConfigurableObject,
                               public LoggableObject {
  Q_OBJECT
 private:
  bool Enable;

 public:
  explicit WidgetLogBackend(const QString& name);
  ~WidgetLogBackend();

  // AbstractLogBackend interface
 public:
  virtual void writeMessage(const QString& str) override;

  // Own
 public:
  void clear();

 private:
  Q_DISABLE_COPY_MOVE(WidgetLogBackend)

 private:
  virtual void loadSettings(void) override;
  void doLoadSettings(void);

 signals:
  void displayLog_signal(const QString& log);
  void clearLogDisplay_signal(void);
};

#endif /* WIDGETLOGBACKEND_H */
