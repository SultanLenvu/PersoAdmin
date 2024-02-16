#ifndef AbstractGUI_AUTHORIZATION_H
#define AbstractGUI_AUTHORIZATION_H

#include <QObject>
#include <QSettings>
#include <QtWidgets>

class AuthorizationGUI final : public QWidget {
  Q_OBJECT
 public:
  QHBoxLayout* MainLayout;

  QGroupBox* AuthorizationMenuGroup;
  QGridLayout* AuthorizationMenuLayout;

  QLabel* LoginLabel;
  QLineEdit* LoginLineEdit;

  QLabel* PasswordLabel;
  QLineEdit* PasswordLineEdit;

  QPushButton* AuthorizePushButton;

  QSpacerItem* ConnectButtonSpacer;

 public:
  explicit AuthorizationGUI(QWidget* parent);
  ~AuthorizationGUI();

 private:
  void create(void);
  void createAuthorizationMenu(void);
};

#endif  // AbstractGUI_AUTHORIZATION_H
