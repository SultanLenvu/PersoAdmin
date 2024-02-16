#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "abstract_gui_subkernel.h"
#include "abstract_manager.h"
#include "global_environment.h"
#include "interaction_system.h"
#include "log_system.h"

class GuiKernel : public QMainWindow {
  Q_OBJECT
 private:
  QSize DesktopGeometry;

  // Верхнее меню
  //===========================================
  QMenu* ServiceMenu;
  QMenu* HelpMenu;

  QAction* SettingsAction;
  QAction* AboutProgramAction;
  //===========================================
  GlobalEnvironment* GEnv;

  std::unique_ptr<InteractionSystem> Interactor;

  std::unique_ptr<QThread> LoggerThread;
  std::unique_ptr<LogSystem> Logger;

  std::vector<std::unique_ptr<AbstractGuiSubkernel>> Subkernels;

  std::unique_ptr<QThread> ManagerThread;
  std::vector<std::unique_ptr<AbstractManager>> Managers;

 public:
  GuiKernel(QWidget* parent = nullptr);
  ~GuiKernel();

  // Функционал для настройки сервера
  void settingsActionTrigger_slot(void);
  void applySettings(void);

 private:
  void loadSettings(void) const;

  void createTopMenu(void);  // Создание верхнего меню
  void createTopMenuActions(void);  // Создание функционала для верхнего меню

  void createMainWindowGui(void);

  void createLoggerInstance(void);
  void createGuiSubkernels(void);
  void createManagersInstance(void);
  void createInteractorInstance(void);

  void registerMetaType(void);

 signals:
  void clearLogDisplay(void);
  void applySettings_signal(void);
};
#endif  // MAINWINDOWKERNEL_H
