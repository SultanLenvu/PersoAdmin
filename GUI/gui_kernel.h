#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "abstract_async_wrapper.h"
#include "abstract_gui_subkernel.h"
#include "abstract_progress_indicator.h"
#include "abstract_return_status_handler.h"
#include "abstract_service_space.h"

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
  std::unique_ptr<AbstractServiceSpace> Service;

  std::unique_ptr<AbstractProgressIndicator> ProgressIndicator;
  std::unique_ptr<AbstractReturnStatusHandler> ReturnStatusHandler;

  std::vector<std::unique_ptr<AbstractGuiSubkernel>> Subkernels;

  std::unique_ptr<QThread> ManagerThread;
  std::vector<std::unique_ptr<AbstractAsyncWrapper>> Managers;

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

  void createServiceSpace(void);
  void createReactions(void);

  void createGuiSubkernels(void);
  void createAsyncInstance(void);

  void registerMetaType(void);

 signals:
  void clearLogDisplay(void);
  void applySettings_signal(void);
};
#endif  // MAINWINDOWKERNEL_H
