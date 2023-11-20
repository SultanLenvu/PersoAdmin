#ifndef PRODUCTIONLINECREATIONMENU_H
#define PRODUCTIONLINECREATIONMENU_H

#include "input_dialog.h"

class ProductionLineCreationMenu : public InputDialog {
  Q_OBJECT

 private:
  QSize DesktopGeometry;

  QVBoxLayout* MainLayout;

  QHBoxLayout* LoginLayout;
  QLabel* LoginLabel;
  QLineEdit* LoginLineEdit;
  QHBoxLayout* PasswordLayout;
  QLabel* PasswordLabel;
  QLineEdit* PasswordLineEdit;
  QPushButton* CreateNewProductionLinePushButton;
  QSpacerItem* VerticalSpacer;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit ProductionLineCreationMenu(QWidget* parent);
  ~ProductionLineCreationMenu();

  virtual void getData(QHash<QString, QString>* data) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineCreationMenu);
  void create(void);
  bool check(void) const;
};

#endif  // PRODUCTIONLINECREATIONMENU_H
