#include "abstract_gui.h"

AbstractGui::AbstractGui(QWidget* parent, AbstractGui::GuiType type)
    : QWidget(parent) {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  Type = type;
}

AbstractGui::~AbstractGui()
{
}

AbstractGui::GuiType AbstractGui::type() {
  return Type;
}
