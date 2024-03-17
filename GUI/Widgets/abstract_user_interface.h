#ifndef ABSTRACTUSERINTERFACE_H
#define ABSTRACTUSERINTERFACE_H

#include <QtWidgets>

class AbstractUserInterface : public QWidget
{
  Q_OBJECT
 public:
  explicit AbstractUserInterface(QWidget* parent = nullptr);
  virtual ~AbstractUserInterface() = default;

 signals:

};

#endif // ABSTRACTUSERINTERFACE_H
