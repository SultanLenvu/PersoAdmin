#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include <QSize>
#include <QtWidgets>

#include "abstract_input_dialog.h"

class AuthorizationDialog : public AbstractInputDialog {
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

  QHBoxLayout* ButtonLayout;
  QPushButton* OkButton;
  QPushButton* CancelButton;

  QSpacerItem* MainLayoutVS;

 public:
  explicit AuthorizationDialog(QWidget* parent = nullptr);
  ~AuthorizationDialog();

  // AbstractInputDialog interface
 public:
  virtual InputDialogType type() const override;
  virtual void getData(StringDictionary& data) const override;

 private:
  Q_DISABLE_COPY_MOVE(AuthorizationDialog);
  void create(void);
};
#endif  // AUTHORIZATIONDIALOG_H
