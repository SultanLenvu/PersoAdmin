#include "idetifier_input_dialog.h"

IdentifierInputDialog::IdentifierInputDialog(QWidget* parent)
    : InputDialog(parent, IdentifierInput) {
  setObjectName("IdentifierInputDialog");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.45, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);
  setWindowTitle("Ввод данных");

  create();
}

IdentifierInputDialog::~IdentifierInputDialog() {}

void IdentifierInputDialog::getData(QHash<QString, QString>* data) const {
  if (!data) {
    return;
  }

  if (check()) {
    data->insert("id", InputData->text());
  } else {
    data->clear();
  }
}

void IdentifierInputDialog::create() {
  MainLayout = new QVBoxLayout();
  setLayout(MainLayout);

  MainLabel = new QLabel("Идентификатор: ");
  MainLayout->addWidget(MainLabel);

  InputData = new QLineEdit();
  MainLayout->addWidget(InputData);

  AcceptButton = new QPushButton("Ввод");
  MainLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool IdentifierInputDialog::check() const {
  bool ok;
  uint32_t t = InputData->text().toUInt(&ok);
  return ok;
}
