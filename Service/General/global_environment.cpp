#include "global_environment.h"

#include <QApplication>
#include <QThread>

GlobalEnvironment::~GlobalEnvironment() {}

GlobalEnvironment* GlobalEnvironment::instance() {
  static GlobalEnvironment context;

  return &context;
}

void GlobalEnvironment::registerObject(NamedObject* obj) {
  QString name = obj->objectName();
  //  assert(!GlobalObjects.contains(name));

  GlobalObjects[name] = obj;

  //  connect(obj, &NamedObject::deleted, this,
  //          &GlobalEnvironment::onRegosteredObjectDestroyed,
  //          Qt::QueuedConnection);
}

NamedObject* GlobalEnvironment::getObject(const QString& name) {
  if (!GlobalObjects.contains(name)) {
    return nullptr;
  }

  return GlobalObjects.value(name);
}

GlobalEnvironment::GlobalEnvironment() {}

void GlobalEnvironment::onRegosteredObjectDestroyed(const QString& name) {
  //  qDebug() << "Объект" << sender()->objectName()
  //           << "удалился из глобального контекста.";
  //  if (QApplication::instance()->thread() != thread()) {
  //    qDebug() << "В отдельном потоке.";
  //  } else {
  //    qDebug() << "В главном потоке.";
  //  }

  GlobalObjects.remove(name);
}
