#include "named_object_factory.h"

NamedObjectFactory::NamedObjectFactory(QThread* thread) {
  assert(thread);

  Thread = thread;
  moveToThread(Thread);

  connect(this, &NamedObjectFactory::create_signal, this,
          &NamedObjectFactory::doCreate, Qt::BlockingQueuedConnection);
}

NamedObjectFactory::~NamedObjectFactory() {}

template <typename T>
typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type
NamedObjectFactory::create(const QString& name) {
  assert(Thread);

  PObject* createdObject = nullptr;

  AuxiliaryBuilder<T> builder;

  emit create_signal(builder, name, &createdObject);
}

void NamedObjectFactory::doCreate(const QString& name,
                                  PObject** createdObject) {}
