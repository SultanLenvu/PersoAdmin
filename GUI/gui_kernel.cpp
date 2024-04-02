#include "gui_kernel.h"
#include "assembly_unit_gui_subkernel.h"
#include "database_gui_subkernel.h"
#include "mainwindow_gui.h"
#include "order_gui_subkernel.h"
#include "production_line_gui_subkernel.h"
#include "programmer_gui_subkernel.h"
#include "progress_indicator.h"
#include "server_connection_gui_subkernel.h"
#include "settings_dialog.h"
#include "status_indicator.h"
#include "sticker_printer_gui_subkernel.h"

GuiKernel::GuiKernel()
    : QMainWindow(nullptr),
      DesktopGeometry(QApplication::primaryScreen()->size()) {
  Service = std::make_unique<ServiceObjectSpace>();
  createReactions();
  Async = std::make_unique<AsyncObjectSpace>();

  createGuiSubkernels();
  createMainWindowGui();
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

void GuiKernel::createReactions() {
  PIndicator = std::unique_ptr<ProgressIndicator>(
      new ProgressIndicator("ProgressIndicator"));

  RSHandler = std::unique_ptr<IStatusIndicator>(
      new StatusIndicator("StatusIndicator"));
}

void GuiKernel::createGuiSubkernels() {
  Subkernels.emplace_back(new DatabaseGuiSubkernel("DatabaseGuiSubkernel"));
  Subkernels.emplace_back(new OrderGuiSubkernel("OrderGuiSubkernel"));
  Subkernels.emplace_back(
      new ProductionLineGuiSubkernel("ProductionLineGuiSubkernel"));
  Subkernels.emplace_back(
      new ServerConnectionGuiSubkernel("ServerConnectionGuiSubkernel"));
  Subkernels.emplace_back(
      new AssemblyUnitGuiSubkernel("AssemblyUnitGuiSubkernel"));
  Subkernels.emplace_back(new ProgrammerGuiSubkernel("ProgrammerGuiSubkernel"));
  Subkernels.emplace_back(
      new StickerPrinterGuiSubkernel("StickerPrinterGuiSubkernel"));
}

void GuiKernel::displaySettingsDialog() {
  emit clearLogDisplay();

  SettingsDialog dialog;
  if (dialog.exec() == QDialog::Rejected) {
    return;
  }
}
