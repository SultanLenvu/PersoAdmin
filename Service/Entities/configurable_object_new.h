#ifndef CONFIGURABLEOBJECTNEW_H
#define CONFIGURABLEOBJECTNEW_H

#include <QObject>

class SettingsApplyBundle;

class ConfigurableObjectNew {
  friend class SettingsApplyBundle;

 private:
  std::unique_ptr<SettingsApplyBundle> Connector;

 public:
  ConfigurableObjectNew();
  virtual ~ConfigurableObjectNew();

 private:
  virtual void loadSettings(void) = 0;
};

class SettingsApplyBundle : public QObject {
  Q_OBJECT

 private:
  ConfigurableObjectNew* Object;

 public:
  SettingsApplyBundle(ConfigurableObjectNew* entity);
  virtual ~SettingsApplyBundle();

 private slots:
  void apply(void);
};

#endif  // CONFIGURABLEOBJECTNEW_H
