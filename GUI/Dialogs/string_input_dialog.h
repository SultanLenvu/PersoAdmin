#ifndef STRINGINPUTDIALOG_H
#define STRINGINPUTDIALOG_H

#include "abstract_input_dialog.h"
#include "abstract_string_checker.h"

class StringInputDialog : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* MainLabel;
  QLineEdit* InputData;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

  std::unique_ptr<AbstractStringChecker> Checker;

 public:
  explicit StringInputDialog(QWidget* parent = nullptr);
  ~StringInputDialog();

 public:  // AbstractInputDialog interface
  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type(void) const override;

  virtual void accept() override;

 public:
  void setLabelText(const QString& text);
  void setChecker(std::unique_ptr<AbstractStringChecker> checker);

 private:
  Q_DISABLE_COPY_MOVE(StringInputDialog);
  void create(void);
};

#endif  // STRINGINPUTDIALOG_H
