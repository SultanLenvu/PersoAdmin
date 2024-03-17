#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include <QObject>

class SettingsApplyBundle;

class ConfigurableObject {
  friend class SettingsApplyBundle;

 private:
  std::unique_ptr<SettingsApplyBundle> Connector;

 public:
  explicit ConfigurableObject();
  virtual ~ConfigurableObject() = default;

 private:
  virtual void loadSettings(void) = 0;
};

class SettingsApplyBundle final : public QObject {
  Q_OBJECT

 private:
  ConfigurableObject* Object;

 public:
  SettingsApplyBundle(ConfigurableObject* entity);
  ~SettingsApplyBundle();

 private slots:
  void apply(void);
};

#endif  // CONFIGURABLEOBJECT_H
