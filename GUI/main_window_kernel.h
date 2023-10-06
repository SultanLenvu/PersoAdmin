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

  UserInteractionSystem* Interactor;

  QThread* ManagerThread;
  AdminManager* Manager;

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
  void createManagerInstance(void);
  void createModels(void);
  void createMatchingTable(void);

 signals:
  void applySettings_signal();

  void connectDatabase_signal(void);
  void disconnectDatabase_signal(void);
  void showDatabaseTable_signal(const QString& name, DatabaseTableModel* model);
  void clearDatabaseTable_signal(const QString& name,
                                 DatabaseTableModel* model);

  void performCustomRequest_signal(const QString& req,
                                   DatabaseTableModel* model);

  void createNewOrder_signal(const QMap<QString, QString>* orderParameterseters,
                             DatabaseTableModel* model);
  void startOrderAssembling_signal(const QString& orderId,
                                   DatabaseTableModel* model);
  void stopOrderAssembling_signal(const QString& orderId,
                                  DatabaseTableModel* model);
  void deleteLastOrder_signal(DatabaseTableModel* model);
  void showOrderTable_signal(DatabaseTableModel* model);

  void createNewProductionLine_signal(
      const QMap<QString, QString>* productionLineParameterseters,
      DatabaseTableModel* model);
  void allocateInactiveProductionLines_signal(const QString& orderId,
                                              DatabaseTableModel* model);
  void shutdownAllProductionLines_signal(DatabaseTableModel* model);
  void deleteLastProductionLine_signal(DatabaseTableModel* model);
  void showProductionLineTable_signal(DatabaseTableModel* model);
  void linkProductionLineWithBox_signal(
      const QMap<QString, QString>* linkParameterseters,
      DatabaseTableModel* model);

  void initIssuers_signal(DatabaseTableModel* model);
  void initTransportMasterKeys_signal(DatabaseTableModel* model);
  void linkIssuerWithMasterKeys_signal(
      DatabaseTableModel* model,
      const QMap<QString, QString>* Parameterseters);
};
#endif  // MAINWINDOWKERNEL_H
