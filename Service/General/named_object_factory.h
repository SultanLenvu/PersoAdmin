#ifndef NAMEDOBJECTFACTORY_H
#define NAMEDOBJECTFACTORY_H

#include <QMetaType>
#include <QObject>
#include <QThread>

#include "abstract_sql_database.h"
#include "named_object.h"

class NamedObjectFactory final : public QObject {
  Q_OBJECT

 private:
  QThread* Thread;
  const QMetaObject* CreatedMetaObject;

 public:
  NamedObjectFactory(QThread* thread);
  ~NamedObjectFactory();

 public:
  template <typename T>
  typename std::enable_if<std::is_base_of<NamedObject, T>::value &&
                              std::is_constructible<T, const QString&>::value,
                          T*>::type
  create(const QString& name) {
    if (!Thread->isRunning()) {
      return nullptr;
    }

    CreatedMetaObject = QMetaType(qRegisterMetaType<T>()).metaObject();

    NamedObject* createdObject;
    bool ok = QMetaObject::invokeMethod(this, "doCreate",
                                        Qt::BlockingQueuedConnection,
                                        qReturnArg(createdObject), name);
    if (!ok) {
      return nullptr;
    }

    return reinterpret_cast<T*>(createdObject);
  }

  template <typename T>
  typename std::enable_if<
      std::is_base_of<NamedObject, T>::value &&
          std::is_constructible<T,
                                const QString&,
                                std::shared_ptr<AbstractSqlDatabase>>::value,
      T*>::type
  create(const QString& name, std::shared_ptr<AbstractSqlDatabase> database) {
    if (!Thread->isRunning()) {
      return nullptr;
    }

    CreatedMetaObject = QMetaType(qRegisterMetaType<T>()).metaObject();

    NamedObject* createdObject;
    bool ok = QMetaObject::invokeMethod(
        this, "doCreate", Qt::BlockingQueuedConnection,
        qReturnArg(createdObject), name, database);
    if (!ok) {
      return nullptr;
    }

    return reinterpret_cast<T*>(createdObject);
  }

 private:
  Q_DISABLE_COPY_MOVE(NamedObjectFactory)

 private slots:
  NamedObject* doCreate(const QString& objectName);
  NamedObject* doCreate(const QString& objectName,
                        std::shared_ptr<AbstractSqlDatabase> database);

 signals:
};

#endif  // NAMEDOBJECTFACTORY_H
