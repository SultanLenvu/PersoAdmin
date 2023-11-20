#include "pan_input_dialog.h"
#include "General/definitions.h"

PanAbstractInputDialog::PanAbstractInputDialog(QWidget* parent)
    : AbstractInputDialog(parent) {
  setObjectName("PanAbstractInputDialog");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.45, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1);
  setWindowTitle("Сканирование стикера");

  create();
}

PanAbstractInputDialog::~PanAbstractInputDialog() {}

void PanAbstractInputDialog::getData(QHash<QString, QString>* data) const {
  if (!data) {
    return;
  }

  QString pan;
  if (check(pan)) {
    data->insert("pan", pan);
  } else {
    data->clear();
  }
}

AbstractInputDialog::DialogType PanAbstractInputDialog::type() const {
  return PanInput;
}

void PanAbstractInputDialog::create() {
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

bool PanAbstractInputDialog::check(QString& pan) const {
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
