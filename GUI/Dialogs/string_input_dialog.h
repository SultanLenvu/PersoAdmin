#ifndef STRINGINPUTDIALOG_H
#define STRINGINPUTDIALOG_H

#include "abstract_input_dialog.h"
#include "abstract_string_checker.h"

class StringInputDialog : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;
  QString ParamName;

  QGridLayout* MainLayout;

  QLabel* MainLabel;
  QLineEdit* InputData;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

  AbstractStringChecker* Checker;

 public:
  explicit StringInputDialog();
  explicit StringInputDialog(const QString& paramName);
  explicit StringInputDialog(const QString& paramName,
                             AbstractStringChecker* checker);
  ~StringInputDialog() = default;

 public:
  void setParamName(const QString& name);

 public:  // AbstractInputDialog interface
  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type(void) const override;

  virtual void accept() override;

 public:
  void setLabelText(const QString& text);
  void setChecker(AbstractStringChecker* checker);

 private:
  Q_DISABLE_COPY_MOVE(StringInputDialog);
  void create(void);
};

#endif  // STRINGINPUTDIALOG_H
