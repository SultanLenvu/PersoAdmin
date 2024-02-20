#include "global_environment.h"

GlobalEnvironment::~GlobalEnvironment() {}

GlobalEnvironment* GlobalEnvironment::instance() {
  static GlobalEnvironment context;

  return &context;
}

void GlobalEnvironment::registerObject(QObject* obj) {
  QString name = obj->objectName();
  assert(!GlobalObjects.contains(name));

  GlobalObjects[name] = obj;
  connect(obj, &QObject::destroyed, this,
          &GlobalEnvironment::onRegosteredObjectDestroyed,
          Qt::BlockingQueuedConnection);
}

QObject* GlobalEnvironment::getObject(const QString& name) {
  if (!GlobalObjects.contains(name)) {
    return nullptr;
  }

  return GlobalObjects.value(name);
}

GlobalEnvironment::GlobalEnvironment() {}

void GlobalEnvironment::onRegosteredObjectDestroyed() {
  GlobalObjects.remove(sender()->objectName());
}
