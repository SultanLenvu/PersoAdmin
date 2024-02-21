#ifndef POBJECT_H
#define POBJECT_H

#include <QObject>

class PObject : public QObject {
  Q_OBJECT
 public:
  explicit PObject(const QString& name);
  virtual ~PObject();

 protected:
  void sendLog(const QString& log) const;

 private:
  PObject();
  Q_DISABLE_COPY_MOVE(PObject)

  void connectDependencies(void);

 signals:
  void logging(const QString& log) const;
};

#endif  // POBJECT_H
