#ifndef MAINWINDOWKERNEL_H
#define MAINWINDOWKERNEL_H

#include <QDebug>
#include <QHash>
#include <QMainWindow>
#include <QRegularExpression>
#include <QSettings>
#include <QSharedPointer>
#include <QString>

#include "Database/database_table_model.h"
#include "General/definitions.h"
#include "General/hash_model.h"
#include "Log/log_system.h"
#include "Management/admin_manager.h"
#include "Transponder/transponder_seed_model.h"
#include "gui.h"
#include "gui_authorization.h"
#include "gui_master.h"
#include "interaction_system.h"

class MainWindowKernel : public QMainWindow {
  Q_OBJECT
 private:
  QRect DesktopGeometry;
  GUI* CurrentGUI;

  QMenu* ServiceMenu;
  QMenu* HelpMenu;

  QAction* AboutProgramAct;
  QAction* RequestAuthorizationGuiAct;

  InteractionSystem* Interactor;

  QThread* ManagerThread;
  AdminManager* Manager;

  QThread* LoggerThread;
  LogSystem* Logger;

  DatabaseTableModel* RandomModel;
  DatabaseTableModel* OrderModel;
  DatabaseTableModel* ProductionLineModel;
  DatabaseTableModel* IssuerModel;
  DatabaseTableModel* TransponderModel;
  DatabaseTableModel* StickerModel;

  HashModel* TransponderData;

  QHash<QString, QString>* MatchingTable;

 public:
  MainWindowKernel(QWidget* parent = nullptr);
  ~MainWindowKernel();

 public slots:
  void on_RequestAuthorizationGuiAct_slot(void);

  // Авторизация
  void on_AuthorizePushButton_slot(void);

  // Функционал для работы с базой данных
  void on_ConnectDatabasePushButton_slot(void);
  void on_DisconnectDatabasePushButton_slot(void);
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

  // Функционал для взаимодействия с сервером
  void on_ReleaseTransponderPushButton_slot(void);
  void on_ConfirmTransponderPushButton_slot(void);
  void on_RereleaseTransponderPushButton_slot(void);
  void on_ConfirmRereleaseTransponderPushButton_slot(void);
  void on_ProductionLineRollbackPushButton_slot(void);

  void on_PrintBoxStickerOnServerPushButton_slot(void);
  void on_PrintLastBoxStickerOnServerPushButton_slot(void);
  void on_PrintPalletStickerOnServerPushButton_slot(void);
  void on_PrintLastPalletStickerOnServerPushButton_slot(void);

  // Функционал для выпуска, возврата и отгрузки транспондеров
  void on_TransponderManualReleasePushButton_slot(void);
  void on_TransponderManualRefundPushButton_slot(void);
  void on_PalletShipmentPushButton_slot(void);

  // Функционал для работы с принтером стикеров
  void on_PrintTransponderStickerPushButton_slot(void);
  void on_PrintBoxStickerPushButton_slot(void);
  void on_PrintPalletStickerPushButton_slot(void);
  void on_ExecStickerPrinterCommandScriptPushButton_slot(void);

  // Функционал для настройки сервера
  void on_ApplySettingsPushButton_slot(void);

  // Отображение данных
  void displayFirmware_slot(QSharedPointer<QFile> firmware);
  void displayTransponderData_slot(
      QSharedPointer<QHash<QString, QString>> transponderData);

 private:
  Q_DISABLE_COPY(MainWindowKernel)
  void loadSettings(void) const;
  void saveSettings(void) const;
  bool checkAuthorizationData(void) const;
  bool checkNewSettings(void) const;
  bool checkNewOrderInput(void) const;
  bool checkNewProductionLineInput(void) const;
  bool checkReleaseTransponderInput(void) const;
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
  void createInteractorInstance(void);
  void createModels(void);
  void createMatchingTable(void);

  void registerMetaType(void);

 signals:
  void applySettings_signal();

  // Логгер
  void loggerClear_signal(void);
  void logging(const QString& log);

  // База данных
  void connectDatabase_signal(void);
  void disconnectDatabase_signal(void);
  void showDatabaseTable_signal(const QString& name, DatabaseTableModel* model);
  void clearDatabaseTable_signal(const QString& name,
                                 DatabaseTableModel* model);

  void performCustomRequest_signal(const QString& req,
                                   DatabaseTableModel* model);

  // Заказы
  void createNewOrder_signal(
      const QSharedPointer<QHash<QString, QString>> orderParameterseters,
      DatabaseTableModel* model);
  void startOrderAssembling_signal(const QString& orderId,
                                   DatabaseTableModel* model);
  void stopOrderAssembling_signal(const QString& orderId,
                                  DatabaseTableModel* model);
  void deleteLastOrder_signal(DatabaseTableModel* model);
  void showOrderTable_signal(DatabaseTableModel* model);

  // Производственные линии
  void createNewProductionLine_signal(
      const QHash<QString, QString>* productionLineParameterseters,
      DatabaseTableModel* model);
  void allocateInactiveProductionLines_signal(const QString& orderId,
                                              DatabaseTableModel* model);
  void shutdownAllProductionLines_signal(DatabaseTableModel* model);
  void deleteLastProductionLine_signal(DatabaseTableModel* model);
  void showProductionLineTable_signal(DatabaseTableModel* model);
  void linkProductionLineWithBox_signal(
      const QHash<QString, QString>* linkParameterseters,
      DatabaseTableModel* model);

  // Тест сервера
  void releaseTransponder_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void confirmTransponderRelease_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void rereleaseTransponder_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void confirmTransponderRerelease_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void rollbackProductionLine_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void printBoxStickerOnServer_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void printLastBoxStickerOnServer_signal();
  void printPalletStickerOnServer_signal(
      const QSharedPointer<QHash<QString, QString>> param);
  void printLastPalletStickerOnServer_signal();

  // Транспондеры
  void releaseTranspondersManually_signal(
      const QSharedPointer<QHash<QString, QString>> param,
      DatabaseTableModel* model);
  void refundTranspondersManually_signal(
      const QSharedPointer<QHash<QString, QString>> param,
      DatabaseTableModel* model);
  void shipPallets_signal(const QSharedPointer<QHash<QString, QString>> param,
                          DatabaseTableModel* model);

  // Заказчики
  void initIssuers_signal(DatabaseTableModel* model);
  void initTransportMasterKeys_signal(DatabaseTableModel* model);
  void linkIssuerWithMasterKeys_signal(
      DatabaseTableModel* model,
      const QHash<QString, QString>* Parameterseters);

  // Принтер
  void printTransponderSticker_signal(const QString& id,
                                      DatabaseTableModel* model);
  void printBoxSticker_signal(const QString& id, DatabaseTableModel* model);
  void printPalletSticker_signal(const QString& id, DatabaseTableModel* model);
  void execPrinterStickerCommandScript_signal(
      const QSharedPointer<QStringList> commandScript);
};
#endif  // MAINWINDOWKERNEL_H
