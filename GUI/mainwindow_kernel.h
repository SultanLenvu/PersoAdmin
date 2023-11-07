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
#include "abstract_gui.h"
#include "authorization_gui.h"
#include "interaction_system.h"
#include "mainwindow_gui.h"

class MainWindowKernel : public QMainWindow {
  Q_OBJECT
 private:
   QSize DesktopGeometry;
   AbstractGUI* CurrentGUI;

   QMenu* ServiceMenu;
   QMenu* HelpMenu;

   QAction* AboutProgramAct;
   QAction* RequestAuthorizationGUIAct;

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
   void requestAuthorizationGUIAct_slot(void);

   // Авторизация
   void authorizePushButton_slot(void);

   // Функционал для работы с базой данных
   void connectDatabasePushButton_slot(void);
   void disconnectDatabasePushButton_slot(void);
   void showDatabaseTablePushButton_slot(void);
   void clearDatabaseTablePushButton_slot(void);
   void transmitCustomRequestPushButton_slot(void);

   // Функционал для работы с заказами
   void createNewOrderPushButton_slot(void);
   void startOrderAssemblingPushButton_slot(void);
   void stopOrderAssemblingPushButton_slot(void);
   void updateOrderViewPushButton_slot(void);
   void deleteLastOrderPushButton_slot(void);

   // Функционал для работы с производственными линиями
   void createNewProductionLinePushButton_slot(void);
   void allocateInactiveProductionLinesPushButton_slot(void);
   void linkProductionLinePushButton_slot(void);
   void deactivateAllProductionLinesPushButton_slot(void);
   void updateProductionLineViewPushButton_slot(void);
   void deleteLastProductionLinePushButton_slot(void);

   // Функционал для работы с транспортными мастер ключами
   void showIssuerTablePushButton_slot(void);
   void initTransportMasterKeysPushButton_slot(void);
   void initIssuerTablePushButton_slot(void);
   void linkIssuerWithKeysPushButton_slot(void);

   // Функционал для взаимодействия с сервером
   void releaseTransponderPushButton_slot(void);
   void confirmTransponderPushButton_slot(void);
   void rereleaseTransponderPushButton_slot(void);
   void confirmRereleaseTransponderPushButton_slot(void);
   void productionLineRollbackPushButton_slot(void);

   void printBoxStickerOnServerPushButton_slot(void);
   void printLastBoxStickerOnServerPushButton_slot(void);
   void printPalletStickerOnServerPushButton_slot(void);
   void printLastPalletStickerOnServerPushButton_slot(void);

   // Функционал для выпуска, возврата и отгрузки транспондеров
   void transponderManualReleasePushButton_slot(void);
   void transponderManualRefundPushButton_slot(void);
   void palletShipmentPushButton_slot(void);

   // Функционал для работы с принтером стикеров
   void printTransponderStickerPushButton_slot(void);
   void printBoxStickerPushButton_slot(void);
   void printPalletStickerPushButton_slot(void);
   void execStickerPrinterCommandScriptPushButton_slot(void);

   // Функционал для настройки сервера
   void applySettingsPushButton_slot(void);

   // Отображение данных
   void displayFirmware_slot(QSharedPointer<QFile> firmware);
   void displayTransponderData_slot(QSharedPointer<QHash<QString, QString>> transponderData);

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

  void createAuthorazationGUI(void);
  void connectAuthorizationGUI(void);

  void createMainWindowGUI(void);
  void connectMainWindowGUI(void);

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
      const QSharedPointer<QHash<QString, QString>>
          productionLineParameterseters,
      DatabaseTableModel* model);
  void allocateInactiveProductionLines_signal(const QString& orderId,
                                              DatabaseTableModel* model);
  void shutdownAllProductionLines_signal(DatabaseTableModel* model);
  void deleteLastProductionLine_signal(DatabaseTableModel* model);
  void showProductionLineTable_signal(DatabaseTableModel* model);
  void linkProductionLineWithBox_signal(
      const QSharedPointer<QHash<QString, QString>> linkParameters,
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
      const QSharedPointer<QHash<QString, QString>> Parameterseters);

  // Принтер
  void printTransponderSticker_signal(const QString& id,
                                      DatabaseTableModel* model);
  void printBoxSticker_signal(const QString& id, DatabaseTableModel* model);
  void printPalletSticker_signal(const QString& id, DatabaseTableModel* model);
  void execPrinterStickerCommandScript_signal(
      const QSharedPointer<QStringList> commandScript);
};
#endif  // MAINWINDOWKERNEL_H
