#ifndef MAINWINDOW_GUI_H
#define MAINWINDOW_GUI_H

#include <QObject>
#include <QtCharts>

#include "abstract_gui.h"

#include "General/definitions.h"

class MainWindowGUI : public AbstractGUI {
  Q_OBJECT

 public:
  QTabWidget* Tabs;

  /* Отображение логов */
  //============================================================
  QGroupBox* LogGroup;
  QVBoxLayout* LogLayout;
  QPlainTextEdit* LogDisplay;
  //============================================================

  /* Интерфейс базы данных */
  //============================================================
  QWidget* DatabaseTab;
  QHBoxLayout* DatabaseMainLayout;

  // Панель упралвения БД
  QGroupBox* DatabaseControlPanelGroup;
  QVBoxLayout* DatabaseControlPanelLayout;

  QPushButton* ConnectDatabasePushButton;
  QPushButton* DisconnectDatabasePushButton;
  QSpacerItem* DatabaseControlPanelVS;

  QComboBox* DatabaseTableChoice;
  QPushButton* ShowDatabaseTablePushButton;
  QSpacerItem* DatabaseControlPanelVS1;

  QPushButton* TransmitCustomRequestPushButton;
  QLineEdit* CustomRequestLineEdit;

  // Отображение записей в БД
  QGroupBox* DatabaseBufferGroup;
  QVBoxLayout* DatabaseBufferLayout;
  QTableView* DatabaseRandomModelView;
  //============================================================

  /* Интерфейс для управления заказами */
  //============================================================
  QWidget* OrderTab;
  QHBoxLayout* OrderTabMainLayout;

  QGroupBox* OrderControlPanel;
  QVBoxLayout* OrderControlPanelLayout;

  QCheckBox* FullPersonalizationCheckBox;
  QHBoxLayout* OrderPanelSubLayout;
  QLabel* PanFilePathLabel;
  QLineEdit* PanFilePathLineEdit;
  QPushButton* PanFileExplorePushButton;
  QHBoxLayout* OrderPanelSubLayout1;
  QLabel* IssuerNameComboLabel;
  QComboBox* IssuerNameComboBox;
  QHBoxLayout* OrderPanelSubLayout2;
  QLabel* TransponderQuantityLabel;
  QLineEdit* TransponderQuantityLineEdit;
  QHBoxLayout* OrderPanelSubLayout3;
  QLabel* BoxCapacityLabel;
  QLineEdit* BoxCapacityLineEdit;
  QHBoxLayout* OrderPanelSublayout4;
  QLabel* PalletCapacityLabel;
  QLineEdit* PalletCapacityLineEdit;
  QHBoxLayout* OrderPanelSublayout5;
  QLabel* TransponderModelLabel;
  QLineEdit* TransponderModelLineEdit;
  QHBoxLayout* OrderPanelSubLayout6;
  QLabel* AccrReferenceLabel;
  QLineEdit* AccrReferenceLineEdit;
  QHBoxLayout* OrderPanelSubLayout7;
  QLabel* EquipmentClassLabel;
  QLineEdit* EquipmentClassLineEdit;
  QHBoxLayout* OrderPanelSubLayout8;
  QLabel* ManufacturerIdLabel;
  QLineEdit* ManufacturerIdLineEdit;
  QPushButton* CreateNewOrderPushButton;
  QSpacerItem* OrderControlPanelVS1;

  QHBoxLayout* OrderIdLayout1;
  QLabel* OrderIdLabel1;
  QLineEdit* OrderIdLineEdit1;
  QPushButton* StartOrderAssemblingPushButton;
  QPushButton* StopOrderAssemblingPushButton;
  QSpacerItem* OrderControlPanelVS2;

  QPushButton* UpdateOrderViewPushButton;

  QGroupBox* OrderTablePanel;
  QVBoxLayout* OrderTablePanelLayout;
  QTableView* OrderTableView;
  //============================================================

  /* Интерфейс для управления линиями производства */
  //============================================================
  QWidget* ProductionLinesTab;
  QHBoxLayout* ProductionLinesTabMainLayout;

  QGroupBox* ProductionLinesControlPanel;
  QVBoxLayout* ProductionLinesControlPanelLayout;

  QPushButton* CreateNewProductionLinePushButton;
  QPushButton* StartProductionLinePushButton;
  QPushButton* StopProductionLinePushButton;
  QPushButton* ShutdownAllProductionLinesPushButton;
  QPushButton* EditProductionLinesPushButton;
  QSpacerItem* ProductionLinesControlPanelVS2;

  QPushButton* UpdateProductionLineViewPushButton;

  QGroupBox* ProductionLineTablePanel;
  QVBoxLayout* ProductionLineTableLayout;
  QTableView* ProductionLineTableView;
  //============================================================

  /* Интерфейс для теста сервера */
  //============================================================
  QWidget* ServerTab;
  QHBoxLayout* ServerTabMainLayout;

  QGroupBox* ServerTabControlPanel;
  QVBoxLayout* ServerTabControlPanelLayout;

  QHBoxLayout* LoginLayout2;
  QLabel* LoginLabel2;
  QLineEdit* LoginLineEdit2;
  QHBoxLayout* PasswordLayout2;
  QLabel* PasswordLabel2;
  QLineEdit* PasswordLineEdit2;
  QHBoxLayout* UcidLayout;
  QLabel* UcidLabel;
  QLineEdit* UcidLineEdit;
  QPushButton* ReleaseTransponderPushButton;
  QPushButton* ConfirmTransponderPushButton;

  QHBoxLayout* RereleaseKeyLayout;
  QComboBox* RereleaseKeyComboBox;
  QLineEdit* RereleaseKeyLineEdit;
  QPushButton* RereleaseTransponderPushButton;
  QPushButton* ConfirmRereleaseTransponderPushButton;
  QPushButton* ProductionLineRollbackPushButton;
  QSpacerItem* ServerTabControlPanelVS;

  QPushButton* PrintBoxStickerOnServerPushButton;
  QPushButton* PrintLastBoxStickerOnServerPushButton;
  QPushButton* PrintPalletStickerOnServerPushButton;
  QPushButton* PrintLastPalletStickerOnServerPushButton;

  QGroupBox* TransponderDisplayPanel;
  QVBoxLayout* TransponderDisplayLayout;
  QTableView* TransponderDataTableView;
  QPlainTextEdit* AssembledFirmwareView;
  //============================================================

  /* Интерфейс для управления транспондерами */
  //============================================================
  QWidget* TransponderTab;
  QHBoxLayout* TransponderTabMainLayout;

  QGroupBox* TransponderControlPanel;
  QVBoxLayout* TransponderControlPanelLayout;
  QHBoxLayout* TransponderControlPanelSublayout;
  QComboBox* ChoiceAnyIdComboBox;
  QLineEdit* AnyIdLineEdit;
  QPushButton* TransponderManualReleasePushButton;
  QPushButton* TransponderManualRefundPushButton;
  QSpacerItem* TransponderControlPanelLayoutVS;

  QPushButton* PalletShipmentPushButton;

  QGroupBox* TransponderViewGroup;
  QVBoxLayout* TransponderViewGroupLayout;
  QTableView* TransponderTableView;
  //============================================================

  /* Интерфейс для управления эмитентами */
  //============================================================
  QWidget* IssuerTab;
  QHBoxLayout* IssuerTabMainLayout;

  // Панель управления
  QGroupBox* IssuerControlPanelGroup;
  QVBoxLayout* IssuerControlPanelLayout;

  QComboBox* IssuerTableChoice;
  QPushButton* ShowIssuerTablePushButton;
  QPushButton* InitTransportMasterKeysPushButton;
  QPushButton* InitIssuerTablePushButton;
  QSpacerItem* TransportKeyVS1;

  QHBoxLayout* IssuerIdLayout1;
  QLabel* IssuerIdLabel1;
  QLineEdit* IssuerIdLineEdit1;
  QHBoxLayout* MasterKeysIdLayout1;
  QComboBox* MasterKeysChoice;
  QLineEdit* MasterKeysLineEdit1;
  QPushButton* LinkIssuerWithKeysPushButton;

  // Отображение записей
  QGroupBox* IssuerViewGroup;
  QVBoxLayout* IssuerTableViewLayout;
  QTableView* IssuerTableView;
  //============================================================

  /* Принтер стикеров */
  //============================================================
  QWidget* StickerTab;
  QHBoxLayout* StickerMainLayout;

  QGroupBox* StickerControlPanel;
  QVBoxLayout* StickerControlPanelLayout;
  QLineEdit* TransponderIdLineEdit;
  QPushButton* PrintTransponderStickerPushButton;
  QLineEdit* BoxIdLineEdit2;
  QPushButton* PrintBoxStickerPushButton;
  QLineEdit* PalletIdLineEdit;
  QPushButton* PrintPalletStickerPushButton;
  QSpacerItem* StickerControlPanelVS;
  QPushButton* ExecStickerPrinterCommandScriptPushButton;

  QGroupBox* StickerDataViewGroup;
  QVBoxLayout* StickerDataViewLayout;
  QTableView* StickerDataTableView;
  QPlainTextEdit* StickerPrinterCommandScriptInput;
  //============================================================

  /* Настройки админпанели */
  //============================================================
  QWidget* SettingsTab;
  QHBoxLayout* SettingsMainLayout;
  QVBoxLayout* SettingsMainSubLayout;
  QPushButton* ApplySettingsPushButton;

  QSpacerItem* SettingsHorizontalSpacer1;
  QSpacerItem* SettingsVerticalSpacer1;

  // Настройки базы данных
  QGroupBox* DatabaseSettingsGroupBox;
  QGridLayout* DatabaseSettingsLayout;
  QLabel* DatabaseIpLabel;
  QLineEdit* DatabaseIpLineEdit;
  QLabel* DatabasePortLabel;
  QLineEdit* DatabasePortLineEdit;
  QLabel* DatabaseNameLabel;
  QLineEdit* DatabaseNameLineEdit;
  QLabel* DatabaseUserNameLabel;
  QLineEdit* DatabaseUserNameLineEdit;
  QLabel* DatabaseUserPasswordLabel;
  QLineEdit* DatabaseUserPasswordLineEdit;
  QLabel* IDatabaseControllerLogEnableLabel;
  QCheckBox* IDatabaseControllerLogEnable;

  // Настройки клиента
  QGroupBox* PersoClientSettingsGroupBox;
  QGridLayout* PersoClientSettingsMainLayout;
  QLabel* PersoClientServerIdLabel;
  QLineEdit* PersoClientServerIpLineEdit;
  QLabel* PersoClientServerPortLabel;
  QLineEdit* PersoClientServerPortLineEdit;

  // Настройки системы логгирования
  QGroupBox* LogSystemSettingsGroupBox;
  QGridLayout* LogSystemSettingsLayout;
  QLabel* LogSystemGlobalEnableLabel;
  QCheckBox* LogSystemGlobalEnableCheckBox;

  QLabel* LogSystemExtendedEnableLabel;
  QCheckBox* LogSystemExtendedEnableCheckBox;

  QWidget* LogSystemProxyWidget1;
  QGridLayout* LogSystemProxyWidget1Layout;

  QLabel* LogSystemDisplayEnableLabel;
  QCheckBox* LogSystemDisplayEnableCheckBox;

  QLabel* LogSystemListenPersoServerLabel;
  QCheckBox* LogSystemListenPersoServerCheckBox;

  QWidget* LogSystemProxyWidget2;
  QGridLayout* LogSystemProxyWidget2Layout;
  QLabel* LogSystemListenIpLabel;
  QLineEdit* LogSystemListenIpLineEdit;
  QLabel* LogSystemListenPortLabel;
  QLineEdit* LogSystemListenPortLineEdit;

  QLabel* LogSystemFileEnableLabel;
  QCheckBox* LogSystemFileEnableCheckBox;

  QWidget* LogSystemProxyWidget3;
  QGridLayout* LogSystemProxyWidget3Layout;
  QLabel* LogSystemFileMaxNumberLabel;
  QLineEdit* LogSystemFileMaxNumberLineEdit;

  // Настройки принтера
  QGroupBox* StickerPrinterSettingsGroupBox;
  QGridLayout* StickerPrinterSettingsMainLayout;
  QLabel* StickerPrinterLibPathLabel;
  QLineEdit* StickerPrinterLibPathLineEdit;
  QPushButton* StickerPrinterLibPathPushButton;
  QLabel* StickerPrinterNameLabel;
  QLineEdit* StickerPrinterNameLineEdit;

 public:
  explicit MainWindowGUI(QWidget* parent);

  virtual void create(void) override;
  virtual void update(void) override;

 public slots:
  void displayLog(const QString& data);
  void clearLogDisplay(void);

 private:
  Q_DISABLE_COPY_MOVE(MainWindowGUI)
  void createTabs(void);

  void createDatabaseTab(void);
  void createOrderTab(void);
  void createProductionLineTab(void);
  void createServerTab(void);
  void createTransponderTab(void);
  void createIssuerTab(void);
  void createStickerTab(void);
  void createSettingsTab(void);

  void createLog(void);

 private slots:
  void on_RereleaseKeyComboBox_slot(const QString& text);

  void on_LogSystemEnableCheckBox_slot(int32_t state);
  void on_LogSystemListenPersoServerCheckBox_slot(int32_t state);
  void on_LogSystemFileEnableCheckBox_slot(int32_t state);

  void on_StickerPrinterLibPathPushButton_slot(void);

 signals:
};

#endif  // MAINWINDOW_GUI_H
