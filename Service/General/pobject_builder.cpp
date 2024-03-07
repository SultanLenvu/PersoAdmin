#include "pobject_builder.h"

PObjectBuilder* PObjectBuilder::instance() {
  static PObjectBuilder instance;

  return &instance;
}

PObjectBuilder::~PObjectBuilder() {}

void PObjectBuilder::setThread(QThread* thread) {
  Thread = thread;
}

PObjectBuilder::PObjectBuilder() : Thread(QThread::currentThread()) {}

// template <typename T, typename... Args>
// typename std::enable_if<std::is_base_of<NamedObject, T>::value, T*>::type
// PObjectBuilder::build(Args&&... args) {
//   assert(Thread);

//  T* obj = new T(std::forward<Args>(args)...);
//  obj->moveToThread(Thread);

//  bool ret = true;
//  QMetaObject::invokeMethod(obj, "init", Qt::BlockingQueuedConnection,
//                            qReturnArg(ret));

//  return ret ? obj : nullptr;
//}
