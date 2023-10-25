#ifndef CUSTOMINPUTDIALOG_H
#define CUSTOMINPUTDIALOG_H

#include <QDialog>
#include <QHash>

class CustomInputDialog : public QDialog {
  Q_OBJECT
 public:
  enum DialogType {
    PalletShipping,
    PanInput,
  };
  Q_ENUM(DialogType);

 protected:
  DialogType Type;

 public:
  explicit CustomInputDialog(QWidget* parent, DialogType type);
  virtual ~CustomInputDialog();

  virtual void getData(QHash<QString, QString>* data) const = 0;

  DialogType type(void) const;

 private:
  Q_DISABLE_COPY(CustomInputDialog);
};

#endif  // CUSTOMINPUTDIALOG_H