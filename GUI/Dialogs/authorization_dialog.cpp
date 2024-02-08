#include "authorization_dialog.h"

AuthorizationDialog::AuthorizationDialog(QWidget* parent)
    : AbstractInputDialog(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем виджеты
  create();

  setWindowTitle("Авторизация");
  adjustSize();
  setFixedSize(size());
}

AuthorizationDialog::~AuthorizationDialog() {}

AbstractInputDialog::InputDialogType AuthorizationDialog::type() const {
  return Authorization;
}

void AuthorizationDialog::getData(StringDictionary& data) const {
  data.insert("login", LoginLineEdit->text());
  data.insert("password", PasswordLineEdit->text());
}

void AuthorizationDialog::create()
{
  MainLayout = new QVBoxLayout();
  setLayout(MainLayout);

  LoginLayout = new QHBoxLayout();
  MainLayout->addLayout(LoginLayout);
  LoginLabel = new QLabel("Логин");
  LoginLayout->addWidget(LoginLabel);
  LoginLineEdit = new QLineEdit();
  LoginLayout->addWidget(LoginLineEdit);

  PasswordLayout = new QHBoxLayout();
  MainLayout->addLayout(PasswordLayout);
  PasswordLabel = new QLabel("Пароль");
  PasswordLayout->addWidget(PasswordLabel);
  PasswordLineEdit = new QLineEdit();
  PasswordLayout->addWidget(PasswordLineEdit);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout);

  OkButton = new QPushButton("Авторизоваться");
  ButtonLayout->addWidget(OkButton);
  connect(OkButton, &QPushButton::clicked, this, &QDialog::accept);

  CancelButton = new QPushButton("Отмена");
  ButtonLayout->addWidget(CancelButton);
  connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);

  MainLayoutVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  MainLayout->addItem(MainLayoutVS);
}
