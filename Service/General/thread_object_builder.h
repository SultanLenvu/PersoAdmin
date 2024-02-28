#ifndef OBJECTTHREADBUILDER_H
#define OBJECTTHREADBUILDER_H

#include <QObject>
#include <QThread>

class ThreadObjectBuilder : public QObject {
  Q_OBJECT
 public:
  QThread* PrevThread;
  QThread* Thread;

 public:
  explicit ThreadObjectBuilder(QThread* thread);
  ~ThreadObjectBuilder();

 public:
  //  template <typename T, typename... Args>
  //  typename std::enable_if<std::is_base_of<QObject, T>::value, T*>::type
  //  build(
  //      Args&&... args);
  template <typename T>
  T* build(const QString& name);

 private:
  //  template <typename T, typename... Args>
  //  typename std::enable_if<std::is_base_of<QObject, T>::value, T*>::type
  //  buildInsideThread(Args&&... args);
  template <typename T>
  T* buildInsideThread(const QString& name);

 private:
  ThreadObjectBuilder();
  Q_DISABLE_COPY_MOVE(ThreadObjectBuilder)
};

#endif // OBJECTTHREADBUILDER_H
