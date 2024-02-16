#ifndef DATABASEUSERINTERFACE_H
#define DATABASEUSERINTERFACE_H

#include <QtWidgets>

class DatabaseUserInterface final : public QWidget {
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
  ~DatabaseUserInterface();

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
