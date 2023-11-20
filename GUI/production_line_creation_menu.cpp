#include "production_line_creation_menu.h"
#include "General/definitions.h"

ProductionLineCreationMenu::ProductionLineCreationMenu(QWidget* parent)
    : AbstractInputDialog(parent) {
  setObjectName("PanAbstractInputDialog");

  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  // Создаем диалоговое окно
  setGeometry(DesktopGeometry.width() * 0.4, DesktopGeometry.height() * 0.45,
              DesktopGeometry.width() * 0.2, DesktopGeometry.height() * 0.1);
  setWindowTitle("Cоздание производственной линии");

  create();
}

ProductionLineCreationMenu::~ProductionLineCreationMenu() {}

void ProductionLineCreationMenu::getData(QHash<QString, QString>* data) const {
  data->insert("login", LoginLineEdit->text());
  data->insert("password", PasswordLineEdit->text());
  data->insert("name", NameLineEdit->text());
  data->insert("surname", SurnameLineEdit->text());
}

AbstractInputDialog::DialogType ProductionLineCreationMenu::type() const {
  return ProductionLineCreation;
}

void ProductionLineCreationMenu::create() {
  MainLayout = new QGridLayout();
  setLayout(MainLayout);

  LoginLabel = new QLabel("Логин: ");
  MainLayout->addWidget(LoginLabel, 0, 0, 1, 1);
  LoginLineEdit = new QLineEdit();
  LoginLineEdit->setMaxLength(PL_LOGIN_MAX_LENGHT);
  MainLayout->addWidget(LoginLineEdit, 0, 1, 1, 1);

  PasswordLabel = new QLabel("Пароль: ");
  MainLayout->addWidget(PasswordLabel, 1, 0, 1, 1);
  PasswordLineEdit = new QLineEdit();
  PasswordLineEdit->setMaxLength(PL_PASSWORD_MAX_LENGHT);
  MainLayout->addWidget(PasswordLineEdit, 1, 1, 1, 1);

  NameLabel = new QLabel("Имя сборщика: ");
  MainLayout->addWidget(NameLabel, 2, 0, 1, 1);
  NameLineEdit = new QLineEdit();
  NameLineEdit->setMaxLength(PL_NAME_MAX_LENGHT);
  MainLayout->addWidget(NameLineEdit, 2, 1, 1, 1);

  SurnameLabel = new QLabel("Фамилия сборщика: ");
  MainLayout->addWidget(SurnameLabel, 3, 0, 1, 1);
  SurnameLineEdit = new QLineEdit();
  SurnameLineEdit->setMaxLength(PL_SURNAME_MAX_LENGHT);
  MainLayout->addWidget(SurnameLineEdit, 3, 1, 1, 1);

  VerticalSpacer =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  MainLayout->addItem(VerticalSpacer, 2, 0, 1, 2);
}

bool ProductionLineCreationMenu::check() const {
  return true;
}
