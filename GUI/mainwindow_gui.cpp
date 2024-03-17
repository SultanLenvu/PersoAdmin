#include "mainwindow_gui.h"
#include "abstract_user_interface.h"
#include "database_user_interface.h"
#include "global_environment.h"
#include "order_user_interface.h"
#include "perso_server_user_interface.h"
#include "production_line_user_interface.h"
#include "sticker_printer_user_interface.h"
#include "widget_log_backend.h"

MainWindowGui::MainWindowGui(QWidget* parent) {
  create();
  connectDependecies();
}

void MainWindowGui::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createTabs();
  createLog();

  MainLayout->setStretch(0, 2);
  MainLayout->setStretch(1, 1);
}

void MainWindowGui::displayLog(const QString& data) {
  if (LogDisplay->toPlainText().length() > 500000)
    LogDisplay->clear();

  if (LogDisplay)
    LogDisplay->appendPlainText(data);
}

void MainWindowGui::clearLogDisplay() {
  LogDisplay->clear();
}

void MainWindowGui::createTabs() {
  Tabs = new QTabWidget();
  MainLayout->addWidget(Tabs);

  AbstractUserInterface* userInterface = new DatabaseUserInterface();
  Tabs->addTab(userInterface, "База данных");

  userInterface = new OrderUserInterface();
  Tabs->addTab(userInterface, "Заказы");

  userInterface = new ProductionLineUserInterface();
  Tabs->addTab(userInterface, "Линии производства");

  userInterface = new PersoServerUserInterface();
  Tabs->addTab(userInterface, "Cервер");

  userInterface = new StickerPrinterUserInterface();
  Tabs->addTab(userInterface, "Стикер принтер");

  Tabs->setCurrentIndex(0);
}

void MainWindowGui::createLog() {
  LogGroup = new QGroupBox("Лог");
  LogGroup->setAlignment(Qt::AlignCenter);
  MainLayout->addWidget(LogGroup);

  LogLayout = new QVBoxLayout();
  LogGroup->setLayout(LogLayout);

  LogDisplay = new QPlainTextEdit();
  LogDisplay->setEnabled(true);
  LogDisplay->setTabletTracking(true);
  LogDisplay->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  LogDisplay->setCenterOnScroll(false);
  LogLayout->addWidget(LogDisplay);
}

void MainWindowGui::connectDependecies() {
  WidgetLogBackend* logger = static_cast<WidgetLogBackend*>(
      GlobalEnvironment::instance()->getObject("WidgetLogBackend"));

  connect(logger, &WidgetLogBackend::clearLogDisplay_signal, this,
          &MainWindowGui::clearLogDisplay);
  connect(logger, &WidgetLogBackend::displayLog_signal, this,
          &MainWindowGui::displayLog);
}
