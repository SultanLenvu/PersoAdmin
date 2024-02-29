#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include "pobject.h"

class ConfigurableObject : public PObject {
  Q_OBJECT
 public:
  explicit ConfigurableObject(const QString& name);
  virtual ~ConfigurableObject();

 public slots:
  virtual void applySettings(void);

 private:
  virtual void loadSettings(void);

 private:
  Q_DISABLE_COPY_MOVE(ConfigurableObject)
  void connectDependencies(void);
};

#endif  // CONFIGURABLEOBJECT_H
