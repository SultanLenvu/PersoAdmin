#ifndef PANINPUTDIALOG_H
#define PANINPUTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

#include "input_dialog.h"

class PanInputDialog : public InputDialog {
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
  explicit PanInputDialog(QWidget* parent);
  ~PanInputDialog();

  virtual void getData(QHash<QString, QString>* data) const override;

 private:
  Q_DISABLE_COPY_MOVE(PanInputDialog);
  void create(void);
  bool check(QString& pan) const;
};

#endif  // PANINPUTDIALOG_H
