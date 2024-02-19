#ifndef PSOBJECT_H
#define PSOBJECT_H

#include <QObject>

class PSObject : public QObject
{
  Q_OBJECT
 public:
  explicit PSObject(const QString& name);
  virtual ~PSObject();

 public slots:
  virtual void applySettings(void);

 protected:
  void sendLog(const QString& log) const;

 private:
  PSObject();
  Q_DISABLE_COPY_MOVE(PSObject)

  void connectDependencies(void);

 signals:
  void logging(const QString& log) const;
};

#endif // PSOBJECT_H
