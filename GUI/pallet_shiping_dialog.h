#ifndef PalletShippingDialog_H
#define PalletShippingDialog_H

#include <QDialog>
#include <QList>
#include <QtWidgets>

#include "custom_input_dialog.h"

class PalletShippingDialog : public CustomInputDialog {
  Q_OBJECT
 private:
  QRect DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* PalletLabel1;
  QLineEdit* FirstPalletId;

  QLabel* PalletLabel2;
  QLineEdit* LastPalletId;

  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit PalletShippingDialog(QWidget* parent);
  ~PalletShippingDialog();

  virtual void getData(QHash<QString, QString>* data) override;

 private:
  Q_DISABLE_COPY(PalletShippingDialog);
  void create(void);
  bool checkInput(void);
};

#endif  // PalletShippingDialog_H
