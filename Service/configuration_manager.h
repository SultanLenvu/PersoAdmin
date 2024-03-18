#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "named_object.h"

class ConfigurationSystem : public NamedObject {
  Q_OBJECT
 public:
  Q_INVOKABLE explicit ConfigurationSystem(const QString& name);
  ~ConfigurationSystem() = default;

 private:
  Q_DISABLE_COPY_MOVE(ConfigurationSystem)

 private:
  void init(void);
  void registerMetaTypes(void);

 signals:
  void applySettings_signal(void);
};

#endif  // CONFIGURATIONMANAGER_H
