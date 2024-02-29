#ifndef POBJECTBUILDER_H
#define POBJECTBUILDER_H

#include <QThread>

#include "pobject.h"

class PObjectBuilder {
 public:
  QThread* Thread;

 public:
  explicit PObjectBuilder(QThread* thread);
  ~PObjectBuilder();

 public:
  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type build(
      Args&&... args) {
    assert(Thread);

    T* obj = new T(std::forward<Args>(args)...);
    obj->moveToThread(Thread);

    bool ret = true;
    QMetaObject::invokeMethod(obj, "init", Qt::BlockingQueuedConnection,
                              qReturnArg(ret));

    return ret ? obj : nullptr;
  }

 private:
  PObjectBuilder();
  Q_DISABLE_COPY_MOVE(PObjectBuilder)
};

#include "pobject_builder.h"

#endif  // POBJECTBUILDER_H
