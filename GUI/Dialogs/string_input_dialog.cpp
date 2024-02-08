#include "string_input_dialog.h"

StringInputDialog::StringInputDialog(QWidget* parent)
    : AbstractInputDialog(parent) {
  // Создаем диалоговое окно
  setWindowTitle("Ввод данных");

  create();

  adjustSize();
  setFixedHeight(size().height());
}

StringInputDialog::~StringInputDialog() {}

void StringInputDialog::getData(StringDictionary& data) const {
  data.insert("id", InputData->text());
}

AbstractInputDialog::InputDialogType StringInputDialog::type() const {
  return StringInput;
}

void StringInputDialog::accept() {
  if (!Checker) {
    QDialog::accept();
    return;
  }

  if (!(*Checker)(InputData->text().toStdString())) {
    QMessageBox::critical(this, "Ошибка", "Некорректный ввод данных.",
                          QMessageBox::Ok);
  }

  QDialog::accept();
}

void StringInputDialog::setLabelText(const QString& text) {
  MainLabel->setText(text);
}

void StringInputDialog::setChecker(
    std::unique_ptr<AbstractStringChecker> checker) {
  Checker = std::move(checker);
}

void StringInputDialog::create() {
  MainLayout = new QGridLayout();
  setLayout(MainLayout);

  MainLabel = new QLabel("Данные: ");
  MainLayout->addWidget(MainLabel, 0, 0, 1, 1);

  InputData = new QLineEdit();
  MainLayout->addWidget(InputData, 0, 1, 1, 1);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout, 1, 0, 1, 2);

  AcceptButton = new QPushButton("Ввод");
  ButtonLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  ButtonLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);
}
