#ifndef PANAbstractInputDialog_H
#define PANAbstractInputDialog_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

#include "abstract_input_dialog.h"

class PanAbstractInputDialog : public AbstractInputDialog {
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
  explicit PanAbstractInputDialog(QWidget* parent);
  ~PanAbstractInputDialog();

  virtual void getData(QHash<QString, QString>* data) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(PanAbstractInputDialog);
  void create(void);
  bool check(QString& pan) const;
};

#endif  // PANAbstractInputDialog_H
