#include "gui.h"

GUI::GUI(QWidget* parent, GUI::GUI_Type type) : QWidget(parent) {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  Type = type;
}

GUI::~GUI()
{
}

GUI::GUI_Type GUI::type()
{
  return Type;
}
