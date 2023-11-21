#ifndef ORDERCREATIONMENU_H
#define ORDERCREATIONMENU_H

#include "abstract_input_dialog.h"

class OrderCreationMenu : public AbstractInputDialog {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  QGridLayout* MainLayout;

  QCheckBox* FullPersonalizationCheckBox;

  QLabel* PanFilePathLabel;
  QLineEdit* PanFilePathLineEdit;
  QPushButton* PanFileExplorePushButton;

  QLabel* IssuerNameComboLabel;
  QComboBox* IssuerNameComboBox;

  QLabel* TransponderQuantityLabel;
  QLineEdit* TransponderQuantityLineEdit;

  QLabel* BoxCapacityLabel;
  QLineEdit* BoxCapacityLineEdit;

  QLabel* PalletCapacityLabel;
  QLineEdit* PalletCapacityLineEdit;

  QLabel* TransponderModelLabel;
  QLineEdit* TransponderModelLineEdit;

  QLabel* AccrReferenceLabel;
  QLineEdit* AccrReferenceLineEdit;

  QLabel* EquipmentClassLabel;
  QLineEdit* EquipmentClassLineEdit;

  QLabel* ManufacturerIdLabel;
  QLineEdit* ManufacturerIdLineEdit;

  QHBoxLayout* ButtonLayout;
  QPushButton* AcceptButton;
  QPushButton* RejectButton;

 public:
  explicit OrderCreationMenu(QWidget* parent);
  ~OrderCreationMenu();

  virtual void getData(QHash<QString, QString>* data, bool& ok) const override;
  virtual DialogType type() const override;

 private:
  Q_DISABLE_COPY_MOVE(OrderCreationMenu);
  void create(void);
  bool check(void) const;

 private slots:
  void panFileExplore_slot(void);
};

#endif  // ORDERCREATIONMENU_H
