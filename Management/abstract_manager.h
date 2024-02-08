#ifndef ABSTRACTMANAGER_H
#define ABSTRACTMANAGER_H

#include <QObject>

class AbstractManager : public QObject
{
  Q_OBJECT
      public:
                explicit AbstractManager(QObject *parent = nullptr);

 signals:

};

#endif // ABSTRACTMANAGER_H
