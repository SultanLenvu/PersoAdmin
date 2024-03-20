#ifndef AbstractInputDialog_H
#define AbstractInputDialog_H

#include <QDialog>
#include <QHash>
#include <QtWidgets>

#include "types.h"

class AbstractInputDialog : public QDialog {
  Q_OBJECT
 public:
  enum InputDialogType {
    PalletShipping,
    PanInput,
    StringInput,
    ProductionLineCreation,
    StartProductionLine,
    OrderCreation,
    ManualReleaseRefund,
    TransponderStickerScan,
    Authorization,
    ConfirmTransponderRerelease,
  };
  Q_ENUM(InputDialogType);

 public:
  explicit AbstractInputDialog() = default;
  virtual ~AbstractInputDialog() = default;

 public:
  virtual void getData(StringDictionary& data) const = 0;
  virtual InputDialogType type(void) const = 0;

 private:
  Q_DISABLE_COPY_MOVE(AbstractInputDialog);

 signals:
};

#endif  // AbstractInputDialog_H
