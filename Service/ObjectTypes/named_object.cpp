#include "global_environment.h"
#include "named_object.h"

NamedObject::NamedObject(const QString& name) : QObject{nullptr} {
  setObjectName(name);

  GlobalEnvironment::instance()->registerObject(this);
}

NamedObject::~NamedObject() {
  emit deleted(objectName());
}
