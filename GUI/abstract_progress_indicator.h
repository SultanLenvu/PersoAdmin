#ifndef ABSTRACTPROGRESSINDICATOR_H
#define ABSTRACTPROGRESSINDICATOR_H

#include "pobject.h"

class AbstractProgressIndicator : public PObject {
  Q_OBJECT
 public:
  explicit AbstractProgressIndicator(const QString& name);
  virtual ~AbstractProgressIndicator();

 public slots:
  virtual void begin(const QString& opname) = 0;
  virtual void finish(const QString& opname) = 0;

 signals:
  void abortCurrentOperation(void);
};

#endif // ABSTRACTPROGRESSINDICATOR_H
