#ifndef ABSTRACT_GUI_SUBKERNEL_H
#define ABSTRACT_GUI_SUBKERNEL_H

#include "named_object.h"

class AbstractGuiSubkernel : public NamedObject {
  Q_OBJECT
 public:
  explicit AbstractGuiSubkernel(const QString& name);
  virtual ~AbstractGuiSubkernel() = default;

 private:
  void connectDependecies(void);

 signals:
  void clearLogDisplay(void);
};

#endif // ABSTRACT_GUI_SUBKERNEL_H
