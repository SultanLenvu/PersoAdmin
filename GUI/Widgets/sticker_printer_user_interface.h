#ifndef STICKERPRINTERUSERINTERFACE_H
#define STICKERPRINTERUSERINTERFACE_H

#include "abstract_user_interface.h"

class StickerPrinterUserInterface : public AbstractUserInterface {
  Q_OBJECT
 private:
  QHBoxLayout* MainLayout;

  QGroupBox* ControlPanel;
  QVBoxLayout* ControlPanelLayout;

  QPushButton* PrintTransponderStickerPushButton;
  QPushButton* PrintBoxStickerPushButton;
  QPushButton* PrintPalletStickerPushButton;
  QSpacerItem* ControlPanelVS;
  QPushButton* ExecCommandScriptPushButton;

  QVBoxLayout* MainSublayout;

  QGroupBox* DisplayDataGroup;
  QVBoxLayout* DisplayDataLayout;
  QTableView* StickerDataTableView;

  QGroupBox* CommandScriptGroup;
  QVBoxLayout* CommandScriptLayout;
  QPlainTextEdit* CommandScriptInput;

 public:
  explicit StickerPrinterUserInterface(QWidget* parent = nullptr);
  ~StickerPrinterUserInterface() = default;

 private:
  void create(void);
  void createControlPanel(void);
  void createDataDisplayGroup(void);

  void connectDependencies(void);

 private slots:
  void execCommandScriptPushButton_slot(void);

 signals:
  void execCommandScript_signal(const std::shared_ptr<QStringList> script);
};

#endif // STICKERPRINTERUSERINTERFACE_H
