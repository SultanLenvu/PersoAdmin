#include "te310_printer.h"

TE310Printer::TE310Printer(QObject* parent) : IStickerPrinter(parent, TE310) {
  setObjectName("TE310Printer");
  loadSetting();

  TscLib = new QLibrary(TscLibPath, this);
  loadTscLib();
}

bool TE310Printer::printTransponderSticker(
    const QMap<QString, QString>* parameters) {
  if (parameters->value("issuer_name").isNull()) {
    emit logging(QString("Отсутсвует название компании-заказчика. Сброс."));
    return false;
  }

  emit logging(QString("Печать стикера транспондера для %1.")
                   .arg(parameters->value("issuer_name")));

  if (!TscLib->isLoaded()) {
    emit logging("Библиотека не загружена. Сброс. ");
    return false;
  }

  if (parameters->value("issuer_name") == "Новое качество дорог") {
    printNkdSticker(parameters);
  } else if (parameters->value("issuer_name") ==
             "Магистраль северной столицы") {
    printZsdSticker(parameters);
  } else {
    emit logging("Получено неизвестное название компании-эмитента. Сброс.");
    return false;
  }

  // Сохраняем данные распечатнного стикера
  LastTransponderSticker = *parameters;

  return true;
}

bool TE310Printer::printLastTransponderSticker() {
  if (LastTransponderSticker.isEmpty()) {
    emit logging("Данные о последнем распечанном стикере отсутствуют. Сброс. ");
    return false;
  }

  return printTransponderSticker(&LastTransponderSticker);
}

bool TE310Printer::printBoxSticker(const QMap<QString, QString>* parameters) {
  if (parameters->value("id").isNull()) {
    emit logging(QString("Отсутсвует идентификатор бокса. Сброс."));
    return false;
  }
  emit logging(
      QString("Печать стикера для бокса %1.").arg(parameters->value("id")));

  openPort(QString("\\\\%1\\TSC TE310 (Network)")
               .arg(QHostInfo::localHostName())
               .toUtf8()
               .constData());
  sendCommand("SIZE 100 mm, 50 mm");
  sendCommand("GAP 2 mm,2 mm");
  sendCommand("REFERENCE 0,0");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand(QString("TEXT 600,30,\"E8.FNT\",0,3,3,2,\"JSC %1\"")
                  .arg(ORGANIZATION_NAME)
                  .toUtf8()
                  .constData());

  sendCommand(QString("TEXT 162,276,\"D.FNT\",0,1,1,2,\"SN: %1 %2 %3\"")
                  .arg(parameters->value("manufacturer_id"),
                       parameters->value("battery_insertation_date"),
                       parameters->value("sn"))
                  .toUtf8()
                  .constData());
  sendCommand("PRINT 1");
  closePort();

  return true;
}

bool TE310Printer::printPalletSticker(
    const QMap<QString, QString>* parameters) {}

void TE310Printer::exec(const QStringList* commandScript) {
  openPort("TSC TE310");

  for (int32_t i = 0; i < commandScript->size(); i++) {
    sendCommand(commandScript->at(i).toUtf8());
  }

  closePort();
}

void TE310Printer::applySetting() {
  emit logging("Применение новых настроек.");

  loadSetting();
  TscLib->setFileName(TscLibPath);
  loadTscLib();
}

void TE310Printer::printNkdSticker(const QMap<QString, QString>* parameters) {
  openPort("TSC TE310");
  sendCommand("SIZE 27 mm, 27 mm");
  sendCommand("GAP 2 mm,2 mm");
  sendCommand("REFERENCE 0,0");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand(QString("TEXT 162,30,\"D.FNT\",0,1,1,2,\"PAN: %1\"")
                  .arg(parameters->value("pan"))
                  .toUtf8()
                  .constData());
  sendCommand(QString("QRCODE "
                      "60,60,H,10,A,0,X204,J1,M2,\"%1\n\r%2%3%4\"")
                  .arg(parameters->value("pan"),
                       parameters->value("manufacturer_id"),
                       parameters->value("battery_insertation_date"),
                       parameters->value("sn"))
                  .toUtf8()
                  .constData());
  sendCommand(QString("TEXT 162,276,\"D.FNT\",0,1,1,2,\"SN: %1 %2 %3\"")
                  .arg(parameters->value("manufacturer_id"),
                       parameters->value("battery_insertation_date"),
                       parameters->value("sn"))
                  .toUtf8()
                  .constData());
  sendCommand("PRINT 1");
  closePort();
}

void TE310Printer::printZsdSticker(const QMap<QString, QString>* parameters) {
  openPort("TSC TE310");
  sendCommand("SIZE 30 mm, 20 mm");
  sendCommand("GAP 2 mm, 1 mm");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand(QString("TEXT 180,12,\"D.FNT\",0,1,1,2,\"SN: %1 %2 %3\"")
                  .arg(parameters->value("manufacturer_id"),
                       parameters->value("battery_insertation_date"),
                       parameters->value("sn"))
                  .toUtf8()
                  .constData());
  sendCommand(QString("BARCODE 18,36,\"128\",144,2,0,2,2,\"%1\"")
                  .arg(parameters->value("pan"))
                  .toUtf8()
                  .constData());
  sendCommand("PRINT 1");
  closePort();
}

void TE310Printer::loadSetting() {
  QSettings settings;

  TscLibPath = settings.value("sticker_printer/library_path").toString();
}

void TE310Printer::loadTscLib() {
  if (TscLib->load()) {
    emit logging("Библиотека загружена.");
    about = (TscAbout)TscLib->resolve("about");
    openPort = (TscOpenPort)TscLib->resolve("openport");
    sendCommand = (TscSendCommand)TscLib->resolve("sendcommand");
    closePort = (TscClosePort)TscLib->resolve("closeport");
  } else {
    emit logging("Не удалось загрузить библиотеку.");

    about = nullptr;
    openPort = nullptr;
    sendCommand = nullptr;
    closePort = nullptr;
  }
}
