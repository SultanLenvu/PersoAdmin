#include "named_object_factory.h"

NamedObjectFactory::NamedObjectFactory(QThread* thread) {
  assert(thread);

  Thread = thread;
  moveToThread(Thread);
}

NamedObjectFactory::~NamedObjectFactory() {}

NamedObject* NamedObjectFactory::doCreate(const QString& objectName) {
  NamedObject* createdObject = reinterpret_cast<NamedObject*>(
      CreatedMetaObject->newInstance(objectName));

  return createdObject;
}

NamedObject* NamedObjectFactory::doCreate(
    const QString& objectName,
    std::shared_ptr<AbstractSqlDatabase> database) {
  NamedObject* createdObject = reinterpret_cast<NamedObject*>(
      CreatedMetaObject->newInstance(objectName, database));

  return createdObject;
}
