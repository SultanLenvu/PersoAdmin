#ifndef GUI_AUTHORIZATION_H
#define GUI_AUTHORIZATION_H

#include <QObject>
#include <QSettings>
#include <QtWidgets>

#include "General/definitions.h"
#include "gui.h"

class AuthorizationGUI : public GUI {
  Q_OBJECT
 public:
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

  virtual void create(void) override;
  virtual void update(void) override;

 private:
  void createAuthorizationMenu(void);

 private slots:
};

#endif  // GUI_AUTHORIZATION_H
