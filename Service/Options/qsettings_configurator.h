#ifndef POBJECTCONFIGURATOR_H
#define POBJECTCONFIGURATOR_H

#include <QObject>

#include "configurable_entity.h"

class QSettingsConfigurator : public QObject {
  Q_OBJECT

 public:
  QSettingsConfigurator(ConfigurableEntity* entity);
  virtual ~QSettingsConfigurator();

 private slots:
  void apply(void);

 private:
  void connectDependencies(void);

 private:
  ConfigurableEntity* Entity;
};

#endif  // POBJECTCONFIGURATOR_H
