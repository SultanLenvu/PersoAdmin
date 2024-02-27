#ifndef GLOBALENVIRONMENT_H
#define GLOBALENVIRONMENT_H

#include <QHash>
#include <QObject>

class GlobalEnvironment : public QObject {
  Q_OBJECT
 private:
  QHash<QString, QObject*> GlobalObjects;

 public:
  ~GlobalEnvironment();
  static GlobalEnvironment* instance(void);

  void registerObject(QObject* obj);
  QObject* getObject(const QString& name);

 private:
  GlobalEnvironment();
  Q_DISABLE_COPY_MOVE(GlobalEnvironment)

 private slots:
  void onRegosteredObjectDestroyed(const QString& name);

 signals:
};

#endif  // GLOBALENVIRONMENT_H