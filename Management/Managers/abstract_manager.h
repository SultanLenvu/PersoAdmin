#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include "configurable_object.h"

class AbstractManager : public ConfigurableObject {
  Q_OBJECT
 public:
  explicit AbstractManager(const QString& name);
  virtual ~AbstractManager();
};

#endif  // ABSTRACTMANAGER_H
