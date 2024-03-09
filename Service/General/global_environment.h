#ifndef GLOBALENVIRONMENT_H
#define GLOBALENVIRONMENT_H

#include <QHash>

#include "named_object.h"

class GlobalEnvironment : public QObject {
  Q_OBJECT
 private:
  QHash<QString, NamedObject*> GlobalObjects;

 public:
  ~GlobalEnvironment();
  static GlobalEnvironment* instance(void);

  void registerObject(NamedObject* obj);
  NamedObject* getObject(const QString& name);

 private:
  GlobalEnvironment();
  Q_DISABLE_COPY_MOVE(GlobalEnvironment)

 private slots:
  void onRegosteredObjectDestroyed(const QString& name);

 signals:
};

#endif  // GLOBALENVIRONMENT_H
