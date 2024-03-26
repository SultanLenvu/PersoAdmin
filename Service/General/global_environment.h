#ifndef GLOBALENVIRONMENT_H
#define GLOBALENVIRONMENT_H

#include <QHash>

#include "named_object.h"

class GlobalEnvironment : public QObject {
  Q_OBJECT
 private:
  QHash<QString, NamedObject*> Objects;
  QHash<QString, std::weak_ptr<NamedObject>> SharedObjects;

 public:
  ~GlobalEnvironment() = default;
  static GlobalEnvironment* instance(void);

  void registerObject(NamedObject* obj);
  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<NamedObject, T>>>
  T* getObject(const QString& name) {
    if (!Objects.contains(name)) {
      return nullptr;
    }

    return static_cast<T*>(Objects.value(name));
  }

  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<NamedObject, T>>>
  void registerSharedObject(std::shared_ptr<T> obj) {
    SharedObjects.insert(obj->objectName(), obj);
  }

  template <typename T,
            typename = std::enable_if_t<std::is_base_of_v<NamedObject, T>>>
  std::shared_ptr<T> getSharedObject(const QString& name) {
    return std::static_pointer_cast<T>(SharedObjects[name].lock());
  }

 private:
  explicit GlobalEnvironment() = default;
  Q_DISABLE_COPY_MOVE(GlobalEnvironment)

 private slots:
  void onRegosteredObjectDestroyed(const QString& name);

 signals:
};

#endif  // GLOBALENVIRONMENT_H
