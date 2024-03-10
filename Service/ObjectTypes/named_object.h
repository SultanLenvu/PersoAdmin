#ifndef NAMED_OBJECT_H
#define NAMED_OBJECT_H

#include <QObject>

class NamedObject : public QObject {
  Q_OBJECT

 public:
  explicit NamedObject(const QString& name);
  virtual ~NamedObject();

 private:
  NamedObject();
  Q_DISABLE_COPY_MOVE(NamedObject)

 signals:
  void deleted(const QString& name) const;
};

#endif  // NAMED_OBJECT_H
