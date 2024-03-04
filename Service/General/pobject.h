#ifndef POBJECT_H
#define POBJECT_H

#include <QObject>

class PObjectBuilder;

class PObject : public QObject {
  Q_OBJECT

  friend class PObjectBuilder;

 private:
  bool Valid;

 public:
  explicit PObject(const QString& name);
  virtual ~PObject();

 public:
  bool valid(void);

 public slots:
  bool init(void);

 protected:
  void sendLog(const QString& log) const;

 private:
  virtual bool initInternals(void);

 private:
  PObject();
  Q_DISABLE_COPY_MOVE(PObject)

  void connectDependencies(void);

 signals:
  void logging(const QString& log) const;
  void deleted(const QString& name) const;
};

#endif  // POBJECT_H
