#ifndef SQLTABLE_H
#define SQLTABLE_H

#include <QObject>

class SqlTable : public QObject {
  Q_OBJECT
 public:
  explicit SqlTable(QObject* parent = nullptr);

 signals:
};

#endif  // SQLTABLE_H
