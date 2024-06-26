#ifndef PRODUCTIONLINECREATIONMENU_H
#define PRODUCTIONLINECREATIONMENU_H

#include "abstract_input_dialog.h"

class ProductionLineCreationDialog final : public AbstractInputDialog {
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
  explicit ProductionLineCreationDialog();
  ~ProductionLineCreationDialog() = default;

 public:
  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type() const override;

  virtual void accept() override;

 private:
  Q_DISABLE_COPY_MOVE(ProductionLineCreationDialog);
  void create(void);
  bool check(void) const;
};

#endif  // PRODUCTIONLINECREATIONMENU_H
