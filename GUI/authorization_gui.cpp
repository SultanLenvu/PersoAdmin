#include "authorization_gui.h"

AuthorizationAbstractGUI::AuthorizationAbstractGUI(QWidget* parent)
    : AbstractGUI(parent, InitialConfiguration) {
  setObjectName("AuthorizationAbstractGUI");
}

void AuthorizationAbstractGUI::create() {
  // Создаем панель управления
  createAuthorizationMenu();

  // Настраиваем пропорции
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void AuthorizationAbstractGUI::update() {}

void AuthorizationAbstractGUI::createAuthorizationMenu() {
  QSettings settings;

  AuthorizationMenuGroup = new QGroupBox("Авторизация");
  AuthorizationMenuGroup->setAlignment(Qt::AlignCenter);
  MainLayout->addWidget(AuthorizationMenuGroup);

  AuthorizationMenuLayout = new QGridLayout();
  AuthorizationMenuGroup->setLayout(AuthorizationMenuLayout);

  LoginLabel = new QLabel("Логин");
  AuthorizationMenuLayout->addWidget(LoginLabel, 0, 0, 1, 1);
  LoginLineEdit = new QLineEdit();
  LoginLineEdit->setText(settings.value("Authorization/Login").toString());
  LoginLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  AuthorizationMenuLayout->addWidget(LoginLineEdit, 0, 1, 1, 1);

  PasswordLabel = new QLabel("Пароль");
  AuthorizationMenuLayout->addWidget(PasswordLabel, 1, 0, 1, 1);
  PasswordLineEdit = new QLineEdit();
  PasswordLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  PasswordLineEdit->setText("12345");
  PasswordLineEdit->setEchoMode(QLineEdit::Password);
  AuthorizationMenuLayout->addWidget(PasswordLineEdit, 1, 1, 1, 1);

  AuthorizePushButton = new QPushButton("Авторизироваться");
  AuthorizationMenuLayout->addWidget(AuthorizePushButton, 5, 0, 1, 2);

  ConnectButtonSpacer =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  AuthorizationMenuLayout->addItem(ConnectButtonSpacer, 6, 0, 1, 2);
}
