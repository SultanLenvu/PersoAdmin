#ifndef SETTINGSAPPLYBUNDLE_H
#define SETTINGSAPPLYBUNDLE_H

#include <QObject>

class ConfigurableObject1;

class SettingsApplyBundle : public QObject {
  Q_OBJECT

 private:
  ConfigurableObject1* Object;

 public:
  SettingsApplyBundle(ConfigurableObject1* entity);
  virtual ~SettingsApplyBundle();

 private slots:
  void apply(void);
};

#endif  // SETTINGSAPPLYBUNDLE_H
