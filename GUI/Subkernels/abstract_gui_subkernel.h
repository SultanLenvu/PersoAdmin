#ifndef ABSTRACT_GUI_SUBKERNEL_H
#define ABSTRACT_GUI_SUBKERNEL_H

#include "abstract_gui.h"
#include "psobject.h"

class AbstractGuiSubkernel : public PSObject {
  Q_OBJECT
 public:
  explicit AbstractGuiSubkernel(const QString& name);
  virtual ~AbstractGuiSubkernel();

 public:
  virtual void connectGui(AbstractGui* gui) = 0;
  virtual void reset(void) = 0;

 private:
  void connectDependecies(void);

 signals:
  void clearLogDisplay_signal(void);
};

#endif // ABSTRACT_GUI_SUBKERNEL_H
