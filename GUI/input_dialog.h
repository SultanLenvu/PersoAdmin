#ifndef InputDialog_H
#define InputDialog_H

#include <QDialog>
#include <QHash>
#include <QtWidgets>

class InputDialog : public QDialog {
  Q_OBJECT
 public:
  enum DialogType {
    PalletShipping,
    PanInput,
    IdentifierInput,
    ProductionLineCreation,
    OrderCreation,
  };
  Q_ENUM(DialogType);

 protected:
  DialogType Type;

 public:
  explicit InputDialog(QWidget* parent, DialogType type);
  virtual ~InputDialog();

  virtual void getData(QHash<QString, QString>* data) const = 0;

  DialogType type(void) const;

 private:
  Q_DISABLE_COPY_MOVE(InputDialog);
};

#endif  // InputDialog_H
