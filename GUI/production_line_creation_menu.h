#ifndef PRODUCTIONLINECREATIONMENU_H
#define PRODUCTIONLINECREATIONMENU_H

#include "abstract_input_dialog.h"

class ProductionLineCreationMenu : public AbstractInputDialog {
  Q_OBJECT

 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* LoginLabel;
  QLineEdit* LoginLineEdit;

  QLabel* PasswordLabel;
  QLineEdit* PasswordLineEdit;

  QLabel* NameLabel;
  QLineEdit* NameLineEdit;

  QLabel* SurnameLabel;
  QLineEdit* SurnameLineEdit;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

  QSpacerItem* VerticalSpacer;

 public:
  explicit ProductionLineCreationMenu(QWidget* paren);
  ~ProductionLineCreationMenu();

  virtual void getData(QHash<QString, QString>* datat, bool& ok) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineCreationMenu);
  void create(void);
  bool check(void) const;
};

#endif  // PRODUCTIONLINECREATIONMENU_H
