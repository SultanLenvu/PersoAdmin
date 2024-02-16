#ifndef PRODUCTIONLINEUSERINTERFACE_H
#define PRODUCTIONLINEUSERINTERFACE_H

#include <QtWidgets>

class ProductionLineUserInterface : public QWidget {
  Q_OBJECT
 private:
  QHBoxLayout* MainLayout;

  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QPushButton* CreatePushButton;
  QPushButton* ActivatePushButton;
  QPushButton* ActivateAllPushButton;
  QPushButton* DeactivatePushButton;
  QPushButton* DeactivateAllPushButton;
  QPushButton* EditPushButton;
  QSpacerItem* ControlPanelVS1;

  QPushButton* GetPushButton;

  QGroupBox* DataPanel;
  QVBoxLayout* DataPanelLayout;
  QTableView* DataTableView;

 public:
  explicit ProductionLineUserInterface(QWidget* parent = nullptr);
  ~ProductionLineUserInterface();

 private:
  void create(void);
  void createControlPanel(void);
  void createDataDisplayGroup(void);

  void connectDependencies(void);

 signals:
};

#endif  // PRODUCTIONLINEUSERINTERFACE_H
