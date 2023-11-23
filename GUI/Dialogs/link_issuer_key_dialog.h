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

  QHash<QString, QString> MatchingTable;

 public:
  explicit LinkIssuerKeyDialog(QWidget* parent);
  ~LinkIssuerKeyDialog();

  virtual void getData(QHash<QString, QString>* data, bool& ok) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(LinkIssuerKeyDialog);
  void create(void);
  void initMatchTable(void);
  bool check() const;
};

#endif  // LINKISSUERKEYDIALOG_H
