#include "gui_kernel.h"
#include "database_async_wrapper.h"
#include "database_gui_subkernel.h"
#include "definitions.h"
#include "global_environment.h"
#include "gui_return_status_handler.h"
#include "internal_service_space.h"
#include "mainwindow_gui.h"
#include "measuring_progress_indicator.h"
#include "order_gui_subkernel.h"
#include "order_manager_async_wrapper.h"
#include "perso_server_async_wrapper.h"
#include "perso_server_gui_subkernel.h"
#include "production_line_gui_subkernel.h"
#include "production_line_manager_async_wrapper.h"
#include "programmer_async_wrapper.h"
#include "programmer_gui_subkernel.h"
#include "settings_dialog.h"
#include "sticker_printer_async_wrapper.h"
#include "sticker_printer_gui_subkernel.h"

GuiKernel::GuiKernel(QWidget* parent) : QMainWindow(parent) {
  setObjectName("GuiKernel");
  GlobalEnvironment::instance()->registerObject(this);

  DesktopGeometry = QApplication::primaryScreen()->size();
  loadSettings();

  createServiceSpace();
  createReactions();

  createAsyncInstance();
  createGuiSubkernels();
  createMainWindowGui();

  registerMetaType();
}

GuiKernel::~GuiKernel() {
  ManagerThread->quit();
  ManagerThread->wait();
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
              DesktopGeometry.width() * 0.8, DesktopGeometry.height() * 0.8);

  // Создаем интерфейс
  setCentralWidget(new MainWindowGui());

  // Создаем верхнее меню
  createTopMenu();
}

void GuiKernel::createServiceSpace() {
  Service = std::unique_ptr<InternalServiceSpace>(
      new InternalServiceSpace("InternalServiceSpace"));
}

void GuiKernel::createReactions() {
  ProgressIndicator = std::unique_ptr<MeasuringProgressIndicator>(
      new MeasuringProgressIndicator("MeasuringProgressIndicator"));

  ReturnStatusHandler = std::unique_ptr<AbstractReturnStatusHandler>(
      new GuiReturnStatusHandler("GuiReturnStatusHandler"));
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

void GuiKernel::createAsyncInstance() {
  Managers.emplace_back(new DatabaseAsyncWrapper("DatabaseAsyncWrapper"));
  Managers.emplace_back(
      new OrderManagerAsyncWrapper("OrderManagerAsyncWrapper"));
  Managers.emplace_back(new ProductionLineManagerAsyncWrapper(
      "ProductionLineManagerAsyncWrapper"));
  Managers.emplace_back(new PersoServerAsyncWrapper("PersoServerAsyncWrapper"));
  Managers.emplace_back(new ProgrammerAsyncWrapper("ProgrammerAsyncWrapper"));
  Managers.emplace_back(new StickerPrinterAsyncWrapper("StickerPrinterAsyncWrapper"));

  ManagerThread = std::unique_ptr<QThread>(new QThread());

  for (auto it1 = Managers.cbegin(), it2 = Managers.cend(); it1 != it2; ++it1) {
    connect(ManagerThread.get(), &QThread::started, it1->get(),
            &AbstractAsyncWrapper::onInstanceThreadStarted,
            Qt::DirectConnection);
    connect(it1->get(), &AbstractAsyncWrapper::executionStatus,
            ReturnStatusHandler.get(), &AbstractReturnStatusHandler::handle,
            Qt::QueuedConnection);
    (*it1)->moveToThread(ManagerThread.get());
  }

  ManagerThread->start();
}

void GuiKernel::registerMetaType() {
  qRegisterMetaType<std::shared_ptr<StringDictionary>>(
      "std::shared_ptr<StringDictionary>");
  qRegisterMetaType<std::shared_ptr<QStringList>>(
      "std::shared_ptr<QStringList>");
  qRegisterMetaType<ReturnStatus>("ReturnStatus");
  qRegisterMetaType<std::shared_ptr<QFile>>("std::shared_ptr<QByteArray>");
}
