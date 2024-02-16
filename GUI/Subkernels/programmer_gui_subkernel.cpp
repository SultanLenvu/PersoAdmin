#include "programmer_gui_subkernel.h"

ProgrammerGuiSubkernel::ProgrammerGuiSubkernel(const QString& name)
    : AbstractGuiSubkernel(name) {
  connectDependecies();
}

ProgrammerGuiSubkernel::~ProgrammerGuiSubkernel() {}

void ProgrammerGuiSubkernel::connectDependecies() {}
