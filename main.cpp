#include "GUI/main_window_kernel.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  MainWindowKernel w;
  w.show();
  return a.exec();
}
