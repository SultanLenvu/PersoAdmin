#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QMainWindow>
#include <QMap>
#include <QRegularExpression>
#include <QSettings>
#include <QString>

#include "Database/database_table_model.h"
#include "General/definitions.h"
#include "Log/log_system.h"
#include "Management/admin_manager.h"
#include "Transponder/transponder_seed_model.h"
#include "gui.h"
#include "gui_authorization.h"
#include "gui_master.h"
#include "user_interaction_system.h"

class MainWindowKernel : public QMainWindow {
  Q_OBJECT
 private:
  QRect DesktopGeometry;
  GUI* CurrentGUI;

  QMenu* ServiceMenu;
  QMenu* HelpMenu;

  QAction* AboutProgramAct;
  QAction* RequestAuthorizationGuiAct;

  AdminManager* Manager;
  UserInteractionSystem* Interactor;

  QThread* LoggerThread;
  LogSystem* Logger;

  DatabaseTableModel* RandomModel;
  DatabaseTableModel* OrderModel;
  DatabaseTableModel* ProductionLineModel;
  DatabaseTableModel* IssuerModel;

  QMap<QString, QString>* MatchingTable;

 public:
  MainWindowKernel(QWidget* parent = nullptr);
  ~MainWindowKernel();

 public slots:
  void on_RequestAuthorizationGuiAct_slot(void);

  // Авторизация
  void on_AuthorizePushButton_slot(void);

  // Функционал для работы с базой данных
  void on_ShowDatabaseTablePushButton_slot(void);
  void on_ClearDatabaseTablePushButton_slot(void);
  void on_TransmitCustomRequestPushButton_slot(void);

  // Функционал для работы с заказами
  void on_CreateNewOrderPushButton_slot(void);
  void on_StartOrderAssemblingPushButton_slot(void);
  void on_StopOrderAssemblingPushButton_slot(void);
  void on_UpdateOrderViewPushButton_slot(void);
  void on_DeleteLastOrderPushButton_slot(void);

  // Функционал для работы с производственными линиями
  void on_CreateNewProductionLinePushButton_slot(void);
  void on_AllocateInactiveProductionLinesPushButton_slot(void);
  void on_LinkProductionLinePushButton_slot(void);
  void on_DeactivateAllProductionLinesPushButton_slot(void);
  void on_UpdateProductionLineViewPushButton_slot(void);
  void on_DeleteLastProductionLinePushButton_slot(void);

  // Функционал для работы с транспортными мастер ключами
  void on_ShowIssuerTablePushButton_slot(void);
  void on_InitTransportMasterKeysPushButton_slot(void);
  void on_InitIssuerTablePushButton_slot(void);
  void on_LinkIssuerWithKeysPushButton_slot(void);

  // Функционал для настройки сервера
  void on_ApplySettingsPushButton_slot(void);

 private:
  void loadSettings(void) const;
  bool checkAuthorizationData(void) const;
  bool checkNewSettings(void) const;
  bool checkNewOrderInput(void) const;
  bool checkNewProductionLineInput(void) const;
  bool checkReleaseTransponderInput(void) const;
  bool checkSearchTransponderInput(void) const;
  bool checkConfirmRereleaseTransponderInput(void) const;
  bool checkLinkIssuerInput(void) const;

  void createTopMenu(void);  // Создание верхнего меню
  void createTopMenuActions(void);  // Создание функционала для верхнего меню

  void createAuthorazationGui(void);
  void connectAuthorizationGui(void);

  void createMasterGui(void);
  void connectMasterGui(void);

  void createLoggerInstance(void);
  void createManager(void);
  void createModels(void);
  void createMatchingTable(void);
};
#endif  // MAINWINDOWKERNEL_H
