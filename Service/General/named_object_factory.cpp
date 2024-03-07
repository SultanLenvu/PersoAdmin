#include "named_object_factory.h"

NamedObjectFactory::NamedObjectFactory(QThread* thread) {
  assert(thread);

  Thread = thread;
  moveToThread(Thread);
}

NamedObjectFactory::~NamedObjectFactory() {}

PObject* NamedObjectFactory::doCreate(const QString& objectName) {
  PObject* createdObject =
      reinterpret_cast<PObject*>(CreatedMetaObject->newInstance(objectName));

  return createdObject;
}
