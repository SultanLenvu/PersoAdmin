#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include <memory>

#include <QObject>

// #include "settings_apply_connector.h"

class ConfigurableObject1 {
  //  friend class SettingsApplyBundle;

 private:
  //  std::unique_ptr<SettingsApplyBundle> Connector;

 public:
  ConfigurableObject1();
  virtual ~ConfigurableObject1();

 private:
  virtual void loadSettings(void) = 0;
};

// class SettingsApplyBundle : public QObject {
//   Q_OBJECT

// private:
//  ConfigurableObject1* Object;

// public:
//  SettingsApplyBundle(ConfigurableObject1* entity);
//  virtual ~SettingsApplyBundle();

// private slots:
//  void apply(void);
//};

#endif  // CONFIGURABLEOBJECT_H
