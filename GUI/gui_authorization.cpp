#include "gui_authorization.h"

AuthorizationGUI::AuthorizationGUI(QWidget* parent)
    : GUI(parent, InitialConfiguration) {
  setObjectName("AuthorizationGUI");
}

void AuthorizationGUI::create() {
  // Создаем панель управления
  createAuthorizationMenu();

  // Настраиваем пропорции
  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void AuthorizationGUI::update() {}

void AuthorizationGUI::createAuthorizationMenu() {
  QSettings settings;

  AuthorizationMenuGroup = new QGroupBox("Авторизация");
  AuthorizationMenuGroup->setAlignment(Qt::AlignCenter);
  MainLayout->addWidget(AuthorizationMenuGroup);

  AuthorizationMenuLayout = new QGridLayout();
  AuthorizationMenuGroup->setLayout(AuthorizationMenuLayout);

  DatabaseIpLabel = new QLabel("IP");
  AuthorizationMenuLayout->addWidget(DatabaseIpLabel, 0, 0, 1, 1);
  DatabaseIpLineEdit = new QLineEdit();
  DatabaseIpLineEdit->setText(settings.value("Database/Ip").toString());
  DatabaseIpLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  AuthorizationMenuLayout->addWidget(DatabaseIpLineEdit, 0, 1, 1, 1);

  DatabasePortLabel = new QLabel("Порт");
  AuthorizationMenuLayout->addWidget(DatabasePortLabel, 1, 0, 1, 1);
  DatabasePortLineEdit = new QLineEdit();
  DatabasePortLineEdit->setText(settings.value("Database/Port").toString());
  DatabasePortLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  AuthorizationMenuLayout->addWidget(DatabasePortLineEdit, 1, 1, 1, 1);

  DatabaseNameLabel = new QLabel("Имя базы данных");
  AuthorizationMenuLayout->addWidget(DatabaseNameLabel, 2, 0, 1, 1);
  DatabaseNameLineEdit = new QLineEdit();
  DatabaseNameLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  DatabaseNameLineEdit->setText(settings.value("Database/Name").toString());
  AuthorizationMenuLayout->addWidget(DatabaseNameLineEdit, 2, 1, 1, 1);

  UserNameLabel = new QLabel("Имя пользователя");
  AuthorizationMenuLayout->addWidget(UserNameLabel, 3, 0, 1, 1);
  DatabaseUserNameLineEdit = new QLineEdit();
  DatabaseUserNameLineEdit->setText(
      settings.value("Database/User/Name").toString());
  DatabaseUserNameLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  AuthorizationMenuLayout->addWidget(DatabaseUserNameLineEdit, 3, 1, 1, 1);

  UserPasswordLabel = new QLabel("Пароль пользователя");
  AuthorizationMenuLayout->addWidget(UserPasswordLabel, 4, 0, 1, 1);
  DatabaseUserPasswordLineEdit = new QLineEdit();
  DatabaseUserPasswordLineEdit->setText(
      settings.value("Database/User/Password").toString());
  DatabaseUserPasswordLineEdit->setEchoMode(QLineEdit::Password);
  DatabaseUserPasswordLineEdit->setMaxLength(AUTH_USER_INPUT_MAX_LENGTH);
  AuthorizationMenuLayout->addWidget(DatabaseUserPasswordLineEdit, 4, 1, 1, 1);

  AuthorizePushButton = new QPushButton("Авторизироваться");
  AuthorizationMenuLayout->addWidget(AuthorizePushButton, 5, 0, 1, 2);

  ConnectButtonSpacer =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  AuthorizationMenuLayout->addItem(ConnectButtonSpacer, 6, 0, 1, 2);
}
