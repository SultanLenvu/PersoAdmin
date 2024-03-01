#include "pobject_builder.h"

EruIluvatar* EruIluvatar::instance() {
  static EruIluvatar instance;

  return &instance;
}

EruIluvatar::~EruIluvatar() {}

void EruIluvatar::setThread(QThread* thread) {
  Thread = thread;
}

EruIluvatar::EruIluvatar() : Thread(QThread::currentThread()) {}

// template <typename T, typename... Args>
// typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type
// EruIluvatar::build(Args&&... args) {
//   assert(Thread);

//  T* obj = new T(std::forward<Args>(args)...);
//  obj->moveToThread(Thread);

//  bool ret = true;
//  QMetaObject::invokeMethod(obj, "init", Qt::BlockingQueuedConnection,
//                            qReturnArg(ret));

//  return ret ? obj : nullptr;
//}
