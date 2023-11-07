#include "pan_input_dialog.h"

PanInputDialog::PanInputDialog(QWidget* parent)
    : InputDialog(parent, PanInput) {
  setObjectName("PanInputDialog");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::screens().first()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.5, DesktopGeometry.height() * 0.5,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);
  setWindowTitle("Сканирование стикера");

  create();
}

PanInputDialog::~PanInputDialog() {}

void PanInputDialog::getData(QHash<QString, QString>* data) const {
  if (!data) {
    return;
  }

  QString pan;
  if (checkInput(pan)) {
    data->insert("pan", pan);
  } else {
    data->clear();
  }
}

void PanInputDialog::create() {
  MainLayout = new QVBoxLayout();
  setLayout(MainLayout);

  MainLabel = new QLabel("Данные стикера");
  MainLayout->addWidget(MainLabel);

  StickerData = new QPlainTextEdit();
  MainLayout->addWidget(StickerData);

  AcceptButton = new QPushButton("Начать");
  MainLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  MainLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}

bool PanInputDialog::checkInput(QString& pan) const {
  QStringList input = StickerData->toPlainText().split("\n");

  if (input.size() == 2) {
    pan = input.at(1);
  } else if (input.size() == 1) {
    pan = input.at(0);
  } else {
    return false;
  }

  if (pan.length() != PAN_CHAR_LENGTH) {
    return false;
  }

  QRegularExpression regex("^[0-9]+$");
  QRegularExpressionMatch match = regex.match(pan);
  return match.hasMatch();
}
