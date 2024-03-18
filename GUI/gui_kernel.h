#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "abstract_gui_subkernel.h"
#include "i_progress_indicator.h"
#include "i_status_indicator.h"
#include "asynchronous_object_space.h"
#include "service_object_space.h"

class GuiKernel final : public QMainWindow {
  Q_OBJECT
 private:
  std::unique_ptr<ServiceObjectSpace> Service;
  std::unique_ptr<AsyncObjectSpace> Async;

  std::vector<std::unique_ptr<AbstractGuiSubkernel>> Subkernels;

  std::unique_ptr<IProgressIndicator> PIndicator;
  std::unique_ptr<IStatusIndicator> RSHandler;

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

 private:
  void createTopMenu(void);  // Создание верхнего меню
  void createTopMenuActions(void);  // Создание функционала для верхнего меню

  void createMainWindowGui(void);
  void createReactions(void);
  void createGuiSubkernels(void);

 private slots:
  void settingsActionTrigger_slot(void);

 signals:
  void clearLogDisplay(void);
};
#endif  // MAINWINDOWKERNEL_H
