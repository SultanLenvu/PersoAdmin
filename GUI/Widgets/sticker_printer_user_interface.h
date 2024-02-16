#ifndef STICKERPRINTERUSERINTERFACE_H
#define STICKERPRINTERUSERINTERFACE_H

#include <QtWidgets>

class StickerPrinterUserInterface : public QWidget {
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

  QGroupBox* DisplayDataGroup;
  QVBoxLayout* DisplayDataLayout;
  QTableView* StickerDataTableView;
  QPlainTextEdit* CommandScriptInput;

 public:
  explicit StickerPrinterUserInterface(QWidget* parent = nullptr);
  ~StickerPrinterUserInterface();

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
