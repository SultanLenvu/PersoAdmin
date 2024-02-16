#ifndef MANUALRELEASEREFUNDDIALOG_H
#define MANUALRELEASEREFUNDDIALOG_H

#include "abstract_input_dialog.h"

class ManualReleaseRefundDialog : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* UnitChoiceLabel;
  QComboBox* UnitChoiceComboBox;

  QLabel* IdLabel;
  QLineEdit* IdLineEdit;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

  StringDictionary MatchingTable;

 public:
  explicit ManualReleaseRefundDialog(QWidget* parent = nullptr);
  ~ManualReleaseRefundDialog();

  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type() const override;

  virtual void accept() override;

 private:
  Q_DISABLE_COPY_MOVE(ManualReleaseRefundDialog);
  void create(void);
  void initMatchTable(void);
  bool check() const;
};

#endif  // MANUALRELEASEREFUNDDIALOG_H
