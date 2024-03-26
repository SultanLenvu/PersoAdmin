#include "global_environment.h"

#include <QApplication>
#include <QThread>

GlobalEnvironment* GlobalEnvironment::instance() {
  static GlobalEnvironment context;

  return &context;
}

void GlobalEnvironment::registerObject(NamedObject* obj) {
  QString name = obj->objectName();
  assert(!Objects.contains(name));

  Objects[name] = obj;

  connect(obj, &NamedObject::deleted, this,
          &GlobalEnvironment::onRegosteredObjectDestroyed,
          Qt::QueuedConnection);
}

void GlobalEnvironment::onRegosteredObjectDestroyed(const QString& name) {
  //  qDebug() << "Объект" << sender()->objectName()
  //           << "удалился из глобального контекста.";
  //  if (QApplication::instance()->thread() != thread()) {
  //    qDebug() << "В отдельном потоке.";
  //  } else {
  //    qDebug() << "В главном потоке.";
  //  }

  SharedObjects.remove(name);
  Objects.remove(name);
}
