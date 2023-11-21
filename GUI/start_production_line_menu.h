#ifndef STARTPRODUCTIONLINEMENU_H
#define STARTPRODUCTIONLINEMENU_H

#include "abstract_input_dialog.h"

class StartProductionLineMenu : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* ProductionLineIdLabel;
  QLineEdit* ProductionLineIdInput;

  QLabel* OrderIdLabel;
  QLineEdit* OrderIdInput;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit StartProductionLineMenu(QWidget* parent);
  ~StartProductionLineMenu();

  virtual void getData(QHash<QString, QString>* data, bool& ok) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(StartProductionLineMenu);
  void create(void);
  bool check() const;
};

#endif  // STARTPRODUCTIONLINEMENU_H
