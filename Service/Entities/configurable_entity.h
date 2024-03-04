#ifndef CONFIGURABLEENTITY_H
#define CONFIGURABLEENTITY_H

#include <QObject>

class ConfigurableEntity {
  friend class QSettingsConfigurator;

 public:
  ConfigurableEntity();
  virtual ~ConfigurableEntity();

 private:
  virtual void loadSettings(void) = 0;

 private:
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

 private:
  QSettingsConfigurator* Configurator;
};

#endif // CONFIGURABLEENTITY_H
