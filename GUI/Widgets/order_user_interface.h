#ifndef ORDERUSERINTERFACE_H
#define ORDERUSERINTERFACE_H

#include <QtWidgets>

class OrderUserInterface : public QWidget {
  Q_OBJECT
 private:
  QHBoxLayout* MainLayout;

  // Панель управления
  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QGroupBox* MainActions;
  QVBoxLayout* MainActionsLayout;
  QPushButton* GetPushButton;
  QPushButton* CreatePushButton;
  QPushButton* StartAssemblingPushButton;
  QPushButton* StopAssemblingPushButton;
  QSpacerItem* MainActionsVS;
  QPushButton* GenerateShipmentRegisterPushButton;

  QSpacerItem* ControlPanelVS;

  QGroupBox* ServiceActions;
  QVBoxLayout* ServiceActionsLayout;

  QPushButton* ReleasePushButton;
  QPushButton* RefundPushButton;
  QPushButton* InitTransportMasterKeysPushButton;
  QPushButton* InitIssuersPushButton;
  QPushButton* LinkIssuerWithKeysPushButton;

  // Отображение данных
  QGroupBox* OrderTablePanel;
  QVBoxLayout* OrderTablePanelLayout;
  QTableView* OrderTableView;

 public:
  explicit OrderUserInterface(QWidget* parent = nullptr);
  ~OrderUserInterface();

 private:
  void create(void);
  void createControlPanel(void);
  void createMainActions(void);
  void createServiceActions(void);
  void createDataDisplayGroup(void);

  void connectDependencies(void);

 signals:
};

#endif  // ORDERUSERINTERFACE_H
