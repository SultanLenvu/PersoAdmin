#include <QApplication>

#include "abstract_input_dialog.h"

AbstractInputDialog::AbstractInputDialog(QWidget* parent) : QDialog(parent) {
  CurrentStatus = NoError;
}

AbstractInputDialog::~AbstractInputDialog() {}
