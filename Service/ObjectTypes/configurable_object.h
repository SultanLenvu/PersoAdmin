#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include <QObject>

class ConfigurationSystemConnection;

class ConfigurableObject {
  friend class ConfigurationSystemConnection;

 private:
  std::unique_ptr<ConfigurationSystemConnection> Connector;

 public:
  explicit ConfigurableObject();
  virtual ~ConfigurableObject() = default;

 private:
  virtual void loadSettings(void) = 0;
};

class ConfigurationSystemConnection final : public QObject {
  Q_OBJECT

 private:
  ConfigurableObject* Object;

 public:
  ConfigurationSystemConnection(ConfigurableObject* entity);
  ~ConfigurationSystemConnection() = default;

 private slots:
  void apply(void);
};

#endif  // CONFIGURABLEOBJECT_H
