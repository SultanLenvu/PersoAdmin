#include "sticker_printer_user_interface.h"
#include "global_environment.h"
#include "sticker_printer_gui_subkernel.h"

StickerPrinterUserInterface::StickerPrinterUserInterface(QWidget* parent)
    : AbstractUserInterface{parent} {
  create();
  connectDependencies();
}

void StickerPrinterUserInterface::create() {
  MainLayout = new QHBoxLayout();
  setLayout(MainLayout);

  createControlPanel();
  createDataDisplayGroup();

  MainLayout->setStretch(0, 1);
  MainLayout->setStretch(1, 3);
}

void StickerPrinterUserInterface::createControlPanel() {
  ControlPanel = new QGroupBox(QString("Панель управления"));
  ControlPanel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  MainLayout->addWidget(ControlPanel);

  ControlPanelLayout = new QVBoxLayout();
  ControlPanel->setLayout(ControlPanelLayout);

  PrintTransponderStickerPushButton =
      new QPushButton("Распечатать стикер для транспондера");
  ControlPanelLayout->addWidget(PrintTransponderStickerPushButton);
  PrintBoxStickerPushButton = new QPushButton("Распечатать стикер для бокса");
  ControlPanelLayout->addWidget(PrintBoxStickerPushButton);
  PrintPalletStickerPushButton =
      new QPushButton("Распечатать стикер для паллеты");
  ControlPanelLayout->addWidget(PrintPalletStickerPushButton);

  ControlPanelVS =
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  ControlPanelLayout->addItem(ControlPanelVS);

  ExecCommandScriptPushButton = new QPushButton("Выполнить скрипт");
  ControlPanelLayout->addWidget(ExecCommandScriptPushButton);
}

void StickerPrinterUserInterface::createDataDisplayGroup() {
  DisplayDataGroup = new QGroupBox(QString("Данные стикера"));
  DisplayDataGroup->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  MainLayout->addWidget(DisplayDataGroup);

  DisplayDataLayout = new QVBoxLayout();
  DisplayDataGroup->setLayout(DisplayDataLayout);

  StickerDataTableView = new QTableView();
  DisplayDataLayout->addWidget(StickerDataTableView);
  CommandScriptInput = new QPlainTextEdit();
  DisplayDataLayout->addWidget(CommandScriptInput);
}

void StickerPrinterUserInterface::connectDependencies() {
  const StickerPrinterGuiSubkernel* pls =
      static_cast<const StickerPrinterGuiSubkernel*>(
          GlobalEnvironment::instance()->getObject(
              "StickerPrinterGuiSubkernel"));

  connect(PrintTransponderStickerPushButton, &QPushButton::clicked, pls,
          &StickerPrinterGuiSubkernel::printTransponderSticker);
  connect(PrintBoxStickerPushButton, &QPushButton::clicked, pls,
          &StickerPrinterGuiSubkernel::printBoxSticker);
  connect(PrintPalletStickerPushButton, &QPushButton::clicked, pls,
          &StickerPrinterGuiSubkernel::printPalletSticker);
  connect(this, &StickerPrinterUserInterface::execCommandScript_signal, pls,
          &StickerPrinterGuiSubkernel::execCommandScript);
}

void StickerPrinterUserInterface::execCommandScriptPushButton_slot() {
  std::shared_ptr<QStringList> script(new QStringList());

  //  script->emplace_back(CommandScriptInput->toPlainText().split("\n"));

  emit execCommandScript_signal(script);
}
