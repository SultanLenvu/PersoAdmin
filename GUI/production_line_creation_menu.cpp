#include "production_line_creation_menu.h"
#include "General/definitions.h"

ProductionLineCreationMenu::ProductionLineCreationMenu(QWidget* parent)
    : InputDialog(parent, ProductionLineCreation) {
  setObjectName("PanInputDialog");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::screens().first()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.4, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.1);
  setWindowTitle("Cоздание производственной линии");

  create();
}

ProductionLineCreationMenu::~ProductionLineCreationMenu() {}

void ProductionLineCreationMenu::create() {
  MainLayout = new QVBoxLayout();
  setLayout(MainLayout);

  LoginLayout = new QHBoxLayout();
  MainLayout->addLayout(LoginLayout);
  LoginLabel = new QLabel("Логин: ");
  LoginLayout->addWidget(LoginLabel);
  LoginLineEdit = new QLineEdit();
  LoginLineEdit->setMaxLength(PRODUCTION_LINE_LOGIN_MAX_LENGTH);
  LoginLayout->addWidget(LoginLineEdit);

  PasswordLayout = new QHBoxLayout();
  MainLayout->addLayout(PasswordLayout);
  PasswordLabel = new QLabel("Пароль: ");
  PasswordLayout->addWidget(PasswordLabel);
  PasswordLineEdit = new QLineEdit();
  PasswordLineEdit->setMaxLength(PRODUCTION_LINE_PASSWORD_MAX_LENGTH);
  PasswordLayout->addWidget(PasswordLineEdit);

  CreateNewProductionLinePushButton = new QPushButton("Создать");
  MainLayout->addWidget(CreateNewProductionLinePushButton);

  VerticalSpacer =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  MainLayout->addItem(VerticalSpacer);
}

bool ProductionLineCreationMenu::check() const {}
