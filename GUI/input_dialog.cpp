#include "input_dialog.h"

InputDialog::InputDialog(QWidget* parent, DialogType type)
    : QDialog(parent) {
  Type = type;
}

InputDialog::~InputDialog() {}

InputDialog::DialogType InputDialog::type() const {
  return Type;
}
