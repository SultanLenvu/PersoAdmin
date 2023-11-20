#ifndef IDENTIFIERINPUTDIALOG_H
#define IDENTIFIERINPUTDIALOG_H

#include "input_dialog.h"

class IdentifierInputDialog : public InputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QVBoxLayout* MainLayout;
  QLabel* MainLabel;
  QLineEdit* InputData;
  QString pan;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit IdentifierInputDialog(QWidget* parent);
  ~IdentifierInputDialog();

  virtual void getData(QHash<QString, QString>* data) const override;

 private:
  Q_DISABLE_COPY_MOVE(IdentifierInputDialog);
  void create(void);
  bool check() const;
};

#endif  // IDENTIFIERINPUTDIALOG_H
