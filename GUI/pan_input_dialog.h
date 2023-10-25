#ifndef PANINPUTDIALOG_H
#define PANINPUTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

#include "General/definitions.h"
#include "custom_input_dialog.h"

class PanInputDialog : public CustomInputDialog {
  Q_OBJECT
 private:
  QRect DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* PanLabel;
  QLineEdit* PanLineEdit;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit PanInputDialog(QWidget* parent);
  ~PanInputDialog();

  virtual void getData(QHash<QString, QString>* data) const override;

 private:
  Q_DISABLE_COPY(PanInputDialog);
  void create(void);
  bool checkInput(void) const;
};

#endif // PANINPUTDIALOG_H
