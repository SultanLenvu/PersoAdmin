#ifndef AbstractInputDialog_H
#define AbstractInputDialog_H

#include <QDialog>
#include <QHash>
#include <QtWidgets>

class AbstractInputDialog : public QDialog {
  Q_OBJECT
 public:
  enum DialogType {
    PalletShipping,
    PanInput,
    IdentifierInput,
    ProductionLineCreation,
    StartProductionLine,
    OrderCreation,
    ManualReleaseRefund,
  };
  Q_ENUM(DialogType);

 public:
  explicit AbstractInputDialog(QWidget* parent);
  virtual ~AbstractInputDialog();

  virtual void getData(QHash<QString, QString>* data, bool& ok) const = 0;
  virtual DialogType type(void) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractInputDialog);
};

#endif  // AbstractInputDialog_H
