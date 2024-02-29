#include "pobject_builder.h"

PObjectBuilder::PObjectBuilder(QThread* thread) : Thread(thread) {}

PObjectBuilder::~PObjectBuilder() {}

// template <typename T, typename... Args>
// typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type
// PObjectBuilder::build(Args&&... args) {
//   assert(Thread);

//  T* obj = new T(std::forward<Args>(args)...);
//  obj->moveToThread(Thread);

//  bool ret = true;
//  QMetaObject::invokeMethod(obj, "init", Qt::BlockingQueuedConnection,
//                            qReturnArg(ret));

//  return ret ? obj : nullptr;
//}
