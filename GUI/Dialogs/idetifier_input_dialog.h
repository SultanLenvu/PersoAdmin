#ifndef IdentifierInputDialog_H
#define IdentifierInputDialog_H

#include "abstract_input_dialog.h"

class IdentifierInputDialog : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QLabel* MainLabel;
  QLineEdit* InputData;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit IdentifierInputDialog(QWidget* parent);
  ~IdentifierInputDialog();

  virtual void getData(QHash<QString, QString>* data, bool& ok) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(IdentifierInputDialog);
  void create(void);
  bool check() const;
};

#endif  // IdentifierInputDialog_H
