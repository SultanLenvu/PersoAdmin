#ifndef TRANSPONDERRERELEASEINPUTDIALOG_H
#define TRANSPONDERRERELEASEINPUTDIALOG_H

#include "abstract_input_dialog.h"
#include "pan_checker.h"
#include "ucid_checker.h"

class ConfirmTransponderRereleaseInputDialog : public AbstractInputDialog
{
  Q_OBJECT
 private:
  PanChecker pchecker;
  UcidChecker uchecker;

  QGridLayout* MainLayout;

  QLabel* PanLabel;
  QLineEdit* PanInput;

  QLabel* UcidLabel;
  QLineEdit* UcidInput;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit ConfirmTransponderRereleaseInputDialog();
  ~ConfirmTransponderRereleaseInputDialog() = default;

 public:  // AbstractInputDialog interface
  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type(void) const override;

  virtual void accept() override;

 private:
  Q_DISABLE_COPY_MOVE(ConfirmTransponderRereleaseInputDialog);
  void create(void);
};

#endif // TRANSPONDERRERELEASEINPUTDIALOG_H
