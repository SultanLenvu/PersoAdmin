#ifndef AbstractGUI_AUTHORIZATION_H
#define AbstractGUI_AUTHORIZATION_H

#include <QObject>
#include <QSettings>
#include <QtWidgets>

#include "General/definitions.h"
#include "abstract_gui.h"

class AuthorizationAbstractGUI : public AbstractGUI {
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
  explicit AuthorizationAbstractGUI(QWidget* parent);

  virtual void create(void) override;
  virtual void update(void) override;

 private:
  Q_DISABLE_COPY(AuthorizationAbstractGUI)
  void createAuthorizationMenu(void);

 private slots:
};

#endif  // AbstractGUI_AUTHORIZATION_H
