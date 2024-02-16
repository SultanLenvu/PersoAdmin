#ifndef PROGRAMMERGUISUBKERNEL_H
#define PROGRAMMERGUISUBKERNEL_H

#include "abstract_gui_subkernel.h"

class ProgrammerGuiSubkernel final : public AbstractGuiSubkernel {
  Q_OBJECT

 public:
  explicit ProgrammerGuiSubkernel(const QString& name);
  ~ProgrammerGuiSubkernel();

 private:
  void connectDependecies(void);
};

#endif // PROGRAMMERGUISUBKERNEL_H
