#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "abstract_gui_subkernel.h"
#include "abstract_progress_indicator.h"
#include "abstract_return_status_handler.h"
#include "asynchronous_object_space.h"
#include "service_object_space.h"

class GuiKernel : public QMainWindow {
  Q_OBJECT
 private:
  std::unique_ptr<ServiceObjectSpace> Service;
  std::unique_ptr<AsynchronousObjectSpace> Async;

  std::vector<std::unique_ptr<AbstractGuiSubkernel>> Subkernels;

  std::unique_ptr<AbstractProgressIndicator> PIndicator;
  std::unique_ptr<AbstractReturnStatusHandler> RSHandler;

  QSize DesktopGeometry;
  // Верхнее меню
  //===========================================
  QMenu* ServiceMenu;
  QMenu* HelpMenu;

  QAction* SettingsAction;
  QAction* AboutProgramAction;
  //===========================================

 public:
  explicit GuiKernel();
  ~GuiKernel() = default;

  // Функционал для настройки сервера
  void settingsActionTrigger_slot(void);

 private:
  void createTopMenu(void);  // Создание верхнего меню
  void createTopMenuActions(void);  // Создание функционала для верхнего меню

  void createMainWindowGui(void);
  void createReactions(void);
  void createGuiSubkernels(void);

 signals:
  void clearLogDisplay(void);
};
#endif  // MAINWINDOWKERNEL_H
