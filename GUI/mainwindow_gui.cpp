#include "mainwindow_gui.h"
#include "global_environment.h"
#include "widget_log_backend.h"

MainWindowGui::MainWindowGui(QWidget* parent) {
  setObjectName("MainWindowGui");
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

  createDatabaseTab();
  createOrderTab();
  createProductionLineTab();
  createServerTab();
  createStickerTab();

  Tabs->setCurrentIndex(0);
}

void MainWindowGui::createDatabaseTab() {
  DatabaseTab = new DatabaseUserInterface();
  Tabs->addTab(DatabaseTab, "База данных");
}

void MainWindowGui::createOrderTab() {
  OrderTab = new OrderUserInterface();
  Tabs->addTab(OrderTab, "Заказы");
}

void MainWindowGui::createProductionLineTab() {
  ProductionLineTab = new ProductionLineUserInterface();
  Tabs->addTab(ProductionLineTab, "Линии производства");
}

void MainWindowGui::createServerTab() {
  ServerTab = new PersoServerUserInterface();
  Tabs->addTab(ServerTab, "Cервер");
}

void MainWindowGui::createStickerTab() {
  StickerPrinterTab = new StickerPrinterUserInterface();
  Tabs->addTab(StickerPrinterTab, "Стикер принтер");
}

void MainWindowGui::createLog() {
  LogGroup = new QGroupBox("Лог");
  LogGroup->setAlignment(Qt::AlignCenter);
  MainLayout->addWidget(LogGroup);

  LogLayout = new QVBoxLayout();
  LogGroup->setLayout(LogLayout);

  LogDisplay = new QPlainTextEdit();
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
