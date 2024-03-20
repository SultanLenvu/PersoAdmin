#include "string_input_dialog.h"

StringInputDialog::StringInputDialog()
    : ParamName("input_data"), Checker(nullptr) {
  create();
}

StringInputDialog::StringInputDialog(const QString& paramName)
    : ParamName(paramName), Checker(nullptr) {
  create();
}

StringInputDialog::StringInputDialog(const QString& paramName,
                                     AbstractStringChecker* checker)
    : ParamName(paramName), Checker(checker) {
  create();
}

void StringInputDialog::getData(StringDictionary& data) const {
  data.insert(ParamName, InputData->text());
}

AbstractInputDialog::InputDialogType StringInputDialog::type() const {
  return StringInput;
}

void StringInputDialog::accept() {
  if (!Checker) {
    QDialog::accept();
    return;
  }

  if (!(*Checker)(InputData->text())) {
    QMessageBox::critical(this, "Ошибка", "Некорректный ввод данных.",
                          QMessageBox::Ok);
    return;
  }

  QDialog::accept();
}

void StringInputDialog::setLabelText(const QString& text) {
  MainLabel->setText(text);
}

void StringInputDialog::setChecker(AbstractStringChecker* checker) {
  Checker = checker;
}

void StringInputDialog::create() {
  setWindowTitle("Ввод данных");

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

  adjustSize();
  setFixedHeight(size().height());
}
