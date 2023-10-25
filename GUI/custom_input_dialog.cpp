#include "custom_input_dialog.h"

CustomInputDialog::CustomInputDialog(QWidget* parent, DialogType type)
    : QDialog(parent) {
  Type = type;
}

CustomInputDialog::~CustomInputDialog() {}

CustomInputDialog::DialogType CustomInputDialog::type() const {
  return Type;
}
