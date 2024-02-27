#ifndef CONFIGURABLEOBJECT_H
#define CONFIGURABLEOBJECT_H

#include "pobject.h"

class ConfigurableObject : public PObject {
  Q_OBJECT
 public:
  explicit ConfigurableObject(const QString& name);
  virtual ~ConfigurableObject();

 public slots:
  void applySettings(void);

 private:
  Q_DISABLE_COPY_MOVE(ConfigurableObject)

  virtual void loadSettings(void);

  void connectDependencies(void);
};

#endif  // CONFIGURABLEOBJECT_H
