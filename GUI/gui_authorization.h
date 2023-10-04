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

  QLabel* DatabaseIpLabel;
  QLineEdit* DatabaseIpLineEdit;

  QLabel* DatabasePortLabel;
  QLineEdit* DatabasePortLineEdit;

  QLabel* DatabaseNameLabel;
  QLineEdit* DatabaseNameLineEdit;

  QLabel* UserNameLabel;
  QLineEdit* DatabaseUserNameLineEdit;

  QLabel* UserPasswordLabel;
  QLineEdit* DatabaseUserPasswordLineEdit;

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
