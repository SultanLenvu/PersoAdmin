#include "gui_kernel.h"
#include "database_gui_subkernel.h"
#include "database_manager.h"
#include "definitions.h"
#include "mainwindow_gui.h"
#include "order_gui_subkernel.h"
#include "order_manager.h"
#include "perso_server_gui_subkernel.h"
#include "perso_server_manager.h"
#include "production_line_gui_subkernel.h"
#include "production_line_manager.h"
#include "programmer_gui_subkernel.h"
#include "programmer_manager.h"
#include "settings_dialog.h"
#include "sticker_printer_gui_subkernel.h"
#include "sticker_printer_manager.h"

GuiKernel::GuiKernel(QWidget* parent) : QMainWindow(parent) {
  DesktopGeometry = QApplication::primaryScreen()->size();
  loadSettings();

  // Создаем глобальную среду для сигналов и слотов объектов
  GEnv = GlobalEnvironment::instance();

  createLoggerInstance();
  createInteractorInstance();

  createManagersInstance();
  createGuiSubkernels();
  createMainWindowGui();

  registerMetaType();
}

GuiKernel::~GuiKernel() {
  ManagerThread->quit();
  ManagerThread->wait();

  LoggerThread->quit();
  LoggerThread->wait();
}

void GuiKernel::settingsActionTrigger_slot() {
  emit clearLogDisplay();

  SettingsDialog dialog;
  connect(&dialog, &SettingsDialog::applyNewSettings, this,
          &GuiKernel::applySettings);

  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
}

void GuiKernel::applySettings() {
  emit applySettings_signal();
}

/*
 * Приватные методы
 */

void GuiKernel::loadSettings() const {
  QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
  QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
  QCoreApplication::setApplicationName(PROGRAM_NAME);

  QSettings::setDefaultFormat(QSettings::IniFormat);
}

void GuiKernel::createTopMenu() {
  menuBar()->clear();
  createTopMenuActions();

  ServiceMenu = menuBar()->addMenu("Сервис");
  ServiceMenu->addAction(SettingsAction);

  HelpMenu = menuBar()->addMenu("Справка");
  HelpMenu->addAction(AboutProgramAction);
}

void GuiKernel::createTopMenuActions() {
  SettingsAction = new QAction("Настройки");
  SettingsAction->setStatusTip("Открыть меню настроек");
  connect(SettingsAction, &QAction::triggered, this,
          &GuiKernel::settingsActionTrigger_slot);

  AboutProgramAction = new QAction("О программе", this);
  AboutProgramAction->setStatusTip("Показать сведения о программе");
}

void GuiKernel::createMainWindowGui() {
  // Настраиваем размер главного окна
  setGeometry(DesktopGeometry.width() * 0.1, DesktopGeometry.height() * 0.1,
              DesktopGeometry.width() * 0.7, DesktopGeometry.height() * 0.7);

  // Создаем интерфейс
  setCentralWidget(new MainWindowGui());

  // Создаем верхнее меню
  createTopMenu();
}

void GuiKernel::createLoggerInstance() {
  Logger = std::unique_ptr<LogSystem>(new LogSystem("LogSystem"));

  LoggerThread = std::unique_ptr<QThread>(new QThread());
  connect(LoggerThread.get(), &QThread::started, Logger.get(),
          &LogSystem::instanceThreadStarted);

  Logger->moveToThread(LoggerThread.get());
  LoggerThread->start();
}

void GuiKernel::createGuiSubkernels() {
  Subkernels.emplace_back(new DatabaseGuiSubkernel("DatabaseGuiSubkernel"));
  Subkernels.emplace_back(new OrderGuiSubkernel("OrderGuiSubkernel"));
  Subkernels.emplace_back(
      new ProductionLineGuiSubkernel("ProductionLineGuiSubkernel"));
  Subkernels.emplace_back(
      new PersoServerGuiSubkernel("PersoServerGuiSubkernel"));
  Subkernels.emplace_back(new ProgrammerGuiSubkernel("ProgrammerGuiSubkernel"));
  Subkernels.emplace_back(
      new StickerPrinterGuiSubkernel("StickerPrinterGuiSubkernel"));
}

void GuiKernel::createManagersInstance() {
  Managers.emplace_back(new DatabaseManager("DatabaseManager"));
  Managers.emplace_back(new OrderManager("OrderManager"));
  Managers.emplace_back(new ProductionLineManager("ProductionLineManager"));
  Managers.emplace_back(new PersoServerManager("PersoServerManager"));
  Managers.emplace_back(new ProgrammerManager("ProgrammerManager"));
  Managers.emplace_back(new StickerPrinterManager("StickerPrinterManager"));

  ManagerThread = std::unique_ptr<QThread>(new QThread());

  for (auto it1 = Managers.cbegin(), it2 = Managers.cend(); it1 != it2; ++it1) {
    connect(ManagerThread.get(), &QThread::started, (*it1).get(),
            &AbstractManager::onInstanceThreadStarted);
    connect(this, &GuiKernel::applySettings_signal, (*it1).get(),
            &AbstractManager::applySettings);
    (*it1)->moveToThread(ManagerThread.get());
  }

  ManagerThread->start();
}

void GuiKernel::createInteractorInstance() {
  Interactor = std::unique_ptr<InteractionSystem>(
      new InteractionSystem("InteractionSystem"));
}

void GuiKernel::registerMetaType() {
  qRegisterMetaType<std::shared_ptr<StringDictionary>>(
      "std::shared_ptr<StringDictionary>");
  qRegisterMetaType<std::shared_ptr<QStringList>>(
      "std::shared_ptr<QStringList>");
  qRegisterMetaType<ReturnStatus>("ReturnStatus");
  qRegisterMetaType<std::shared_ptr<QFile>>("std::shared_ptr<QByteArray>");
}
