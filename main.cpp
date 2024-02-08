#include "GUI/gui_kernel.h"

#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  GuiKernel w;
  w.show();
  return a.exec();
}
