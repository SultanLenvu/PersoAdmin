#ifndef MAINWINDOW_GUI_H
#define MAINWINDOW_GUI_H

#include <QWidget>
#include <QtCharts>

class MainWindowGui : public QWidget {
  Q_OBJECT

 public:
  QHBoxLayout* MainLayout;
  QTabWidget* Tabs;

  /* Отображение логов */
  //============================================================
  QGroupBox* LogGroup;
  QVBoxLayout* LogLayout;
  QPlainTextEdit* LogDisplay;
  //============================================================

  //  std::vector<AbstractUserInterface*> UserInterfaces;

 public:
  explicit MainWindowGui(QWidget* parent = nullptr);
  ~MainWindowGui() = default;

 public slots:
  void displayLog(const QString& data);
  void clearLogDisplay(void);

 private:
  Q_DISABLE_COPY_MOVE(MainWindowGui)
  void create(void);
  void createTabs(void);
  void createLog(void);

  void connectDependecies(void);

 signals:
};

#endif  // MAINWINDOW_GUI_H
