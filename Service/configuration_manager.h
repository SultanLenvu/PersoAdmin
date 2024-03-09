#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "named_object.h"

class ConfigurationManager : public NamedObject {
  Q_OBJECT
 public:
  explicit ConfigurationManager(const QString& name);
  ~ConfigurationManager() = default;

 private:
  Q_DISABLE_COPY_MOVE(ConfigurationManager)

 private:
  void registerMetaTypes(void);

 signals:
  void applySettings_signal(void);
};

#endif  // CONFIGURATIONMANAGER_H
