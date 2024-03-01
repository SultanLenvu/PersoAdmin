#ifndef POBJECTBUILDER_H
#define POBJECTBUILDER_H

#include <QThread>

#include "pobject.h"

class PObjectBuilder {
 public:
  QThread* Thread;

 public:
  static PObjectBuilder* instance(void);
  ~PObjectBuilder();

 public:
  void setThread(QThread* thread);

  template <typename T, typename... Args>
  typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type create(
      Args&&... args) {
    assert(Thread);

    if (!Thread->isRunning()) {
      Thread->start();
    }

    T* obj = new T(std::forward<Args>(args)...);
    obj->moveToThread(Thread);

    bool ret = true;
    QMetaObject::invokeMethod(obj, "init", Qt::BlockingQueuedConnection,
                              qReturnArg(ret));

    return ret ? obj : nullptr;
  }

 private:
  //  PObjectBuilder(QThread* thread);
  PObjectBuilder();
  Q_DISABLE_COPY_MOVE(PObjectBuilder)
};

#endif  // POBJECTBUILDER_H
