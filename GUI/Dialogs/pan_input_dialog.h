#ifndef PanInputDialog_H
#define PanInputDialog_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

#include "abstract_input_dialog.h"

class PanInputDialog final : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QVBoxLayout* MainLayout;
  QLabel* MainLabel;
  QPlainTextEdit* StickerData;
  QString pan;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit PanInputDialog();
  ~PanInputDialog() = default;

 public:
  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type() const override;

  virtual void accept() override;

 private:
  Q_DISABLE_COPY_MOVE(PanInputDialog);
  void create(void);
  bool check(void) const;
};

#endif  // PanInputDialog_H
