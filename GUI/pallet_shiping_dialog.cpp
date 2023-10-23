#include "pallet_shiping_dialog.h"

PalletShippingDialog::PalletShippingDialog(QWidget* parent)
    : CustomInputDialog(parent, PalletShipping) {
  setObjectName("PalletShippingDialog");
  setWindowTitle("Отгрузка");

  DesktopGeometry = QApplication::desktop()->screenGeometry();
  setGeometry(DesktopGeometry.width() * 0.5, DesktopGeometry.height() * 0.5,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.05);

  create();
}

PalletShippingDialog::~PalletShippingDialog() {}

void PalletShippingDialog::getData(QHash<QString, QString>* data) {
  if (checkInput()) {
    data->insert("first_pallet_id", FirstPalletId->text());
    data->insert("last_pallet_id", LastPalletId->text());
  } else {
    data->clear();
  }
}

void PalletShippingDialog::create() {
  MainLayout = new QGridLayout(this);
  setLayout(MainLayout);

  PalletLabel1 = new QLabel("Паллеты с");
  MainLayout->addWidget(PalletLabel1, 0, 0, 1, 1);

  FirstPalletId = new QLineEdit();
  MainLayout->addWidget(FirstPalletId, 0, 1, 1, 1);

  PalletLabel2 = new QLabel("по");
  MainLayout->addWidget(PalletLabel2, 0, 2, 1, 1);

  LastPalletId = new QLineEdit();
  MainLayout->addWidget(LastPalletId, 0, 3, 1, 1);

  AcceptButton = new QPushButton("Отгрузить");
  MainLayout->addWidget(AcceptButton, 1, 0, 1, 2);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(RejectButton, 1, 2, 1, 2);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool PalletShippingDialog::checkInput() {
  if ((FirstPalletId->text().toUInt() == 0) ||
      (LastPalletId->text().toUInt() == 0)) {
    return false;
  }

  if (FirstPalletId->text().toInt() > LastPalletId->text().toInt()) {
    return false;
  }

  return true;
}
