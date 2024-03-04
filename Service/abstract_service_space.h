#ifndef ABSTRACTSERVICESPACE_H
#define ABSTRACTSERVICESPACE_H

#include <QObject>

class AbstractServiceSpace : public QObject {
  Q_OBJECT
 public:
  AbstractServiceSpace(const QString& name);
  virtual ~AbstractServiceSpace();

 public:
};

#endif // ABSTRACTSERVICESPACE_H
