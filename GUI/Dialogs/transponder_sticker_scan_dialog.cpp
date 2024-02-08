#include "transponder_sticker_scan_dialog.h"
#include "General/definitions.h"

TransponderStickerScanDialog::TransponderStickerScanDialog(QWidget* parent)
    : AbstractInputDialog(parent) {
  // Считываем размеры дисплея
  DesktopGeometry = QApplication::primaryScreen()->size();

  setWindowTitle("Сканирование стикера");

  create();
  adjustSize();
  setFixedSize(size());

  Regex.setPattern("^[0-9]+$");
}

TransponderStickerScanDialog::~TransponderStickerScanDialog() {}

AbstractInputDialog::InputDialogType TransponderStickerScanDialog::type()
    const {
  return TransponderStickerScan;
}

void TransponderStickerScanDialog::getData(StringDictionary& data) const {
  QStringList input = StickerData->toPlainText().split("\n");
  if (!checkInput(input)) {
    data.clear();
    return;
  }

  if (input.size() == 2) {
    data.insert("transponder_pan", input.at(0));
    data.insert("transponder_sn", input.at(1));
  } else if (input.size() == 1) {
    data.insert("transponder_pan", input.at(0));
  }
}

void TransponderStickerScanDialog::create()
{
  MainLayout = new QVBoxLayout();
  setLayout(MainLayout);

  MainLabel = new QLabel("Данные стикера:");
  MainLayout->addWidget(MainLabel);

  StickerData = new QPlainTextEdit();
  MainLayout->addWidget(StickerData);

  ButtonLayout = new QHBoxLayout();
  MainLayout->addLayout(ButtonLayout);

  AcceptButton = new QPushButton("Начать");
  ButtonLayout->addWidget(AcceptButton);
  connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);

  RejectButton = new QPushButton("Отмена");
  ButtonLayout->addWidget(RejectButton);
  connect(RejectButton, &QPushButton::clicked, this, &QDialog::reject);

  MainLayoutVS =
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);
  MainLayout->addItem(MainLayoutVS);
}

bool TransponderStickerScanDialog::checkInput(QStringList& input) const
{
  if (input.size() == 2) {
    return checkPan(input[0]) && checkSn(input[1]);
  } else if (input.size() == 1) {
    return checkPan(input[0]);
  } else {
    return false;
  }
}

bool TransponderStickerScanDialog::checkPan(const QString& pan) const
{
  if (pan.length() != PAN_CHAR_LENGTH) {
    return false;
  }

  return Regex.match(pan).hasMatch();
}

bool TransponderStickerScanDialog::checkSn(const QString& sn) const
{
  if (sn.length() != SERIAL_NUMBER_DS_CHAR_LENGTH) {
    return false;
  }

  return Regex.match(sn).hasMatch();
}
