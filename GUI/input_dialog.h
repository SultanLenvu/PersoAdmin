#ifndef InputDialog_H
#define InputDialog_H

#include <QDialog>
#include <QHash>

class InputDialog : public QDialog
{
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
   explicit InputDialog(QWidget* parent, DialogType type);
   virtual ~InputDialog();

   virtual void getData(QHash<QString, QString>* data) const = 0;

   DialogType type(void) const;

 private:
   Q_DISABLE_COPY(InputDialog);
};

#endif // InputDialog_H
