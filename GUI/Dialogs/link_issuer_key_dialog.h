#ifndef LINKISSUERKEYDIALOG_H
#define LINKISSUERKEYDIALOG_H

#include "abstract_input_dialog.h"

class LinkIssuerKeyDialog : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* IssuerIdLabel;
  QLineEdit* IssuerIdLineEdit;

  QLabel* KeyGroupLabel;
  QComboBox* KeyChoiceComboBox;
  QLineEdit* KeyGroupLineEdit;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

  StringDictionary MatchingTable;

 public:
  explicit LinkIssuerKeyDialog(QWidget* parent = nullptr);
  ~LinkIssuerKeyDialog();

  virtual void getData(StringDictionary& data) const override;
  virtual InputDialogType type() const override;

  virtual void accept() override;

 private:
  Q_DISABLE_COPY_MOVE(LinkIssuerKeyDialog);
  void create(void);
  void initMatchTable(void);
  bool check() const;
};

#endif  // LINKISSUERKEYDIALOG_H
