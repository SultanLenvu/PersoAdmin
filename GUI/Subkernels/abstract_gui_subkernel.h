#ifndef ABSTRACT_GUI_SUBKERNEL_H
#define ABSTRACT_GUI_SUBKERNEL_H

#include "pobject.h"

class AbstractGuiSubkernel : public PObject {
  Q_OBJECT
 public:
  explicit AbstractGuiSubkernel(const QString& name);
  virtual ~AbstractGuiSubkernel();

 private:
  void connectDependecies(void);

 signals:
  void clearLogDisplay(void);
};

#endif // ABSTRACT_GUI_SUBKERNEL_H
