#ifndef TRANSPONDERSTICKERDIALOG_H
#define TRANSPONDERSTICKERDIALOG_H

#include <QSize>

#include "abstract_input_dialog.h"

class TransponderStickerScanDialog : public AbstractInputDialog {
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

  QSpacerItem* MainLayoutVS;

  QRegularExpression Regex;

 public:
  explicit TransponderStickerScanDialog(QWidget* parent);
  ~TransponderStickerScanDialog();

  // AbstractInputDialog interface
 public:
  virtual InputDialogType type() const override;
  virtual void getData(StringDictionary& data) const override;

 private:
  Q_DISABLE_COPY(TransponderStickerScanDialog);

  void create(void);
  bool checkInput(QStringList& input) const;
  bool checkPan(const QString& pan) const;
  bool checkSn(const QString& sn) const;
};

#endif  // TRANSPONDERSTICKERDIALOG_H
