#include "global_environment.h"
#include "named_object.h"

NamedObject::NamedObject(const QString& name) : QObject{nullptr} {
  setObjectName(name);

  GlobalEnvironment::instance()->registerObject(this);
}

QString NamedObject::name() {
  return objectName();
}

NamedObject::~NamedObject() {
  emit deleted(objectName());
}
