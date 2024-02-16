#ifndef MAINWINDOW_GUI_H
#define MAINWINDOW_GUI_H

#include <QWidget>
#include <QtCharts>

#include "database_user_interface.h"
#include "order_user_interface.h"
#include "perso_server_user_interface.h"
#include "production_line_user_interface.h"
#include "sticker_printer_user_interface.h"

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

  /* Интерфейс персо-сервера */
  DatabaseUserInterface* DatabaseTab;

  /* Интерфейс для управления заказами */
  OrderUserInterface* OrderTab;

  /* Интерфейс для управления линиями производства */
  ProductionLineUserInterface* ProductionLineTab;

  /* Интерфейс персо-сервера */
  PersoServerUserInterface* ServerTab;

  /* Принтер стикеров */
  StickerPrinterUserInterface* StickerPrinterTab;

 public:
  explicit MainWindowGui(QWidget* parent = nullptr);

 public slots:
  void displayLog(const QString& data);
  void clearLogDisplay(void);

 private:
  Q_DISABLE_COPY_MOVE(MainWindowGui)
  void create(void);
  void createTabs(void);

  void createDatabaseTab(void);
  void createOrderTab(void);
  void createProductionLineTab(void);
  void createTransponderTab(void);
  void createServerTab(void);
  void createIssuerTab(void);
  void createStickerTab(void);
  void createLog(void);

  void connectDependecies(void);

 signals:
};

#endif  // MAINWINDOW_GUI_H
