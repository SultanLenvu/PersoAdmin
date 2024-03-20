#include "confirm_transponder_rerelease_input_dialog.h"

ConfirmTransponderRereleaseInputDialog::
    ConfirmTransponderRereleaseInputDialog() {
  create();
}

void ConfirmTransponderRereleaseInputDialog::getData(
    StringDictionary& data) const {
  data.insert("transponder_pan", PanInput->text());
  data.insert("transponder_ucid", UcidInput->text());
}

AbstractInputDialog::InputDialogType
ConfirmTransponderRereleaseInputDialog::type() const {
  return ConfirmTransponderRerelease;
}

void ConfirmTransponderRereleaseInputDialog::accept() {
  if (!pchecker(PanInput->text()) || (!uchecker(UcidInput->text()))) {
    QMessageBox::critical(this, "Ошибка", "Некорректный ввод данных.",
                          QMessageBox::Ok);
    return;
  }

  QDialog::accept();
}

void ConfirmTransponderRereleaseInputDialog::create() {
  setWindowTitle("Ввод данных");

  MainLayout = new QGridLayout();
  setLayout(MainLayout);

  PanLabel = new QLabel("PAN: ");
  MainLayout->addWidget(PanLabel, 0, 0, 1, 1);

  PanInput = new QLineEdit();
  MainLayout->addWidget(PanInput, 0, 1, 1, 1);

  UcidLabel = new QLabel("UCID: ");
  MainLayout->addWidget(UcidLabel, 1, 0, 1, 1);

  UcidInput = new QLineEdit();
  MainLayout->addWidget(UcidInput, 1, 1, 1, 1);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout, 1, 0, 1, 2);

  AcceptButton = new QPushButton("Ввод");
  ButtonLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this,
          &ConfirmTransponderRereleaseInputDialog::accept);

  RejectButton = new QPushButton("Отмена");
  ButtonLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);

  adjustSize();
  setFixedHeight(size().height());
}
