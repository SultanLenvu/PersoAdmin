#ifndef PANINPUTDIALOG_H
#define PANINPUTDIALOG_H

#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QtWidgets>

#include "General/definitions.h"
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
  Q_DISABLE_COPY(PanInputDialog);
  void create(void);
  bool checkInput(QString& pan) const;
};

#endif // PANINPUTDIALOG_H
