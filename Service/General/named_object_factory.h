#ifndef NAMEDOBJECTFACTORY_H
#define NAMEDOBJECTFACTORY_H

#include <QObject>
#include <QThread>

#include "pobject.h"

class NamedObjectFactory final : public QObject {
 private:
  template <typename T, typename... Args>
  class AuxiliaryBuilder {
    AuxiliaryBuilder() = default;
    ~AuxiliaryBuilder() = default;

    PObject* build(const QString& name);
  };

 private:
  QThread* Thread;

 public:
  NamedObjectFactory(QThread* thread);
  ~NamedObjectFactory();

 public:
  template <typename T>
  typename std::enable_if<std::is_base_of<PObject, T>::value, T*>::type create(
      const QString& name);

 public:
  Q_DISABLE_COPY_MOVE(NamedObjectFactory)

 private slots:
  template <typename T>
  void doCreate(AuxiliaryBuilder<T> builder, const QString& name,
                PObject** createdObject);

 signals:
  template <typename T>
  void create_signal(AuxiliaryBuilder<T> builder, const QString& name,
                     PObject** createdObject);
};

#endif  // NAMEDOBJECTFACTORY_H
