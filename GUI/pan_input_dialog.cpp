#include "pan_input_dialog.h"

PanInputDialog::PanInputDialog(QWidget* parent)
    : CustomInputDialog(parent, PanInput) {
  setObjectName("PanInputDialog");
  setWindowTitle("Введите PAN");

  DesktopGeometry = QApplication::desktop()->screenGeometry();
  setGeometry(DesktopGeometry.width() * 0.5, DesktopGeometry.height() * 0.5,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.05);

  create();
}

PanInputDialog::~PanInputDialog() {}

void PanInputDialog::getData(QHash<QString, QString>* data) const {
  if (checkInput()) {
    data->insert("pan", PanLineEdit->text());
  } else {
    data->clear();
  }
}

void PanInputDialog::create() {
  MainLayout = new QGridLayout();
  setLayout(MainLayout);

  PanLabel = new QLabel("PAN:");
  MainLayout->addWidget(PanLabel, 0, 0, 1, 1);

  PanLineEdit = new QLineEdit();
  PanLineEdit->setMaxLength(PAN_CHAR_LENGTH);
  MainLayout->addWidget(PanLineEdit, 0, 1, 1, 1);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout, 1, 0, 1, 2);

  AcceptButton = new QPushButton("Ок");
  ButtonLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  ButtonLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool PanInputDialog::checkInput() const {
  if (PanLineEdit->text().length() != PAN_CHAR_LENGTH) {
    return false;
  }

  QRegularExpression regex("^[0-9]+$");
  QRegularExpressionMatch match = regex.match(PanLineEdit->text());
  return match.hasMatch();
}
