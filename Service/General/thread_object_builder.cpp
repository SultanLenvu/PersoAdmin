#include "thread_object_builder.h"

ThreadObjectBuilder::ThreadObjectBuilder(QThread* thread)
    : QObject{nullptr}, Thread(thread) {
  PrevThread = QThread::currentThread();
}

ThreadObjectBuilder::~ThreadObjectBuilder() {}

// template <typename T, typename... Args>
// typename std::enable_if<std::is_base_of<QObject, T>::value, T*>::type
// ThreadObjectBuilder::build(Args&&... args) {
//   assert(Thread);

//  moveToThread(Thread);
//  QMetaObject::invokeMethod(this, "buildInsideThread", Qt::DirectConnection,
//                            std::forward<Args>(args)...);
//  moveToThread(PrevThread);
//}

// template <typename T, typename... Args>
// typename std::enable_if<std::is_base_of<QObject, T>::value, T*>::type
// ThreadObjectBuilder::buildInsideThread(Args&&... args) {
//   T* obj = new T(std::forward<Args>(args)...);

//  return obj;
//}

template <typename T>
T* ThreadObjectBuilder::build(const QString& name) {
  assert(Thread);

  moveToThread(Thread);
  QMetaObject::invokeMethod(this, "buildInsideThread", Qt::DirectConnection,
                            name);
  moveToThread(PrevThread);
}

template <typename T>
T* ThreadObjectBuilder::buildInsideThread(const QString& name) {
  T* obj = new T(name);

  return obj;
}
