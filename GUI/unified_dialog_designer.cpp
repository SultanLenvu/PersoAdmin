#include "unified_dialog_designer.h"

UnifiedDialogDesigner::UnifiedDialogDesigner(QWidget* parent)
    : QDialog(parent) {
  setObjectName("UnifiedDialogDesigner");

  MainLayout = new QGridLayout(this);
  setLayout(MainLayout);

  DesktopGeometry = QApplication::desktop()->screenGeometry();
}

UnifiedDialogDesigner::~UnifiedDialogDesigner() {}

void UnifiedDialogDesigner::createPalletShippingDialog() {
  setWindowTitle("Отгрузка");
  InputData.clear();
  InputWidgets.clear();
  show();
  setGeometry(DesktopGeometry.width() * 0.5, DesktopGeometry.height() * 0.5,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.05);

  QLabel* palletWidget1 = new QLabel("Паллеты с");
  MainLayout->addWidget(palletWidget1, 0, 0, 1, 1);

  QLineEdit* firstPalletId = new QLineEdit();
  InputWidgets.insert("first_pallet_id", firstPalletId);
  MainLayout->addWidget(firstPalletId, 0, 1, 1, 1);

  QLabel* palletWidget2 = new QLabel("по");
  MainLayout->addWidget(palletWidget2, 0, 2, 1, 1);

  QLineEdit* lastPalletId = new QLineEdit();
  InputWidgets.insert("last_pallet_id", lastPalletId);
  MainLayout->addWidget(lastPalletId, 0, 3, 1, 1);

  QPushButton* acceptButton = new QPushButton("Отгрузить");
  MainLayout->addWidget(acceptButton, 1, 0, 1, 2);
  connect(acceptButton, &QPushButton::clicked, this,
          &UnifiedDialogDesigner::accept);

  QPushButton* rejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(rejectButton, 1, 2, 1, 2);
  connect(rejectButton, &QPushButton::clicked, this,
          &UnifiedDialogDesigner::reject);
}

const QHash<QString, QString>* UnifiedDialogDesigner::getInputData() {
  return &InputData;
}

void UnifiedDialogDesigner::accept() {
  InputData.insert(
      "first_pallet_id",
      dynamic_cast<QLineEdit*>(InputWidgets.value("first_pallet_id"))->text());
  InputData.insert(
      "last_pallet_id",
      dynamic_cast<QLineEdit*>(InputWidgets.value("last_pallet_id"))->text());

  clear();
  QDialog::accept();
}

void UnifiedDialogDesigner::reject() {
  InputData.clear();

  clear();
  QDialog::reject();
}

void UnifiedDialogDesigner::clear() {
  QList<QWidget*> childWidgets = findChildren<QWidget*>();
  for (QWidget* childWidget : childWidgets) {
    childWidget->deleteLater();
  }
}
