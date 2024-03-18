#ifndef DATABASEUSERINTERFACE_H
#define DATABASEUSERINTERFACE_H

#include "abstract_user_interface.h"

class DatabaseUserInterface final : public AbstractUserInterface {
  Q_OBJECT
 private:
  QHBoxLayout* MainLayout;

  // Панель управления
  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QPushButton* ConnectPushButton;
  QPushButton* DisconnectPushButton;
  QSpacerItem* DatabaseControlPanelVS;

  QComboBox* TableChoice;
  QPushButton* GetTablePushButton;
  QSpacerItem* DatabaseControlPanelVS1;

  QPushButton* ExecCustomRequestPushButton;
  QLineEdit* CustomRequestLineEdit;

  // Отображение записей
  QGroupBox* DataDisplayGroup;
  QVBoxLayout* DataDisplayLayout;
  QTableView* RecordTableView;

 public:
  explicit DatabaseUserInterface(QWidget* parent = nullptr);
  ~DatabaseUserInterface() = default;

 private:
  void create(void);
  void createControlPanel(void);
  void createDataDisplayGroup(void);

  void connectDependecies(void);

 private slots:
  void getTablePushButton_slot(void);

 signals:
  void getTable_signal(const QString& name);
};

#endif  // DATABASEUSERINTERFACE_H
