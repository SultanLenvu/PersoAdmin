#include <QHostAddress>
#include <QPrinterInfo>
#include <QTextCodec>
#include <QtPrintSupport>

#include "te310_printer.h"

TE310Printer::TE310Printer(const QString& name)
    : NamedObject(name), LoggableObject(name) {
  doLoadSettings();

  loadTscLib();
}

ReturnStatus TE310Printer::checkConfig() {
  sendLog("Проверка конфигурации.");

  if (!UseEthernet) {
    QList<QString> printers = QPrinterInfo::availablePrinterNames();
    if (std::find_if(printers.begin(), printers.end(), [this](const QString p) {
          return p == SystemName;
        }) == printers.end()) {
      sendLog("Не найден драйвер операционной системы. ");
      return ReturnStatus::StickerPrinterDriverMissed;
    }
  }

  if (!TscLib->isLoaded()) {
    sendLog("Не удалось загрузить библиотеку.");
    return ReturnStatus::StickerPrinterLibraryMissing;
  }

  sendLog("Проверка конфигурации успешно завершена.");
  return ReturnStatus::NoError;
}

ReturnStatus TE310Printer::printTransponderSticker(
    const StringDictionary& param) {
  sendLog(QString("Печать стикера для транспондера."));

  // Проверка параметров
  if (param.value("issuer_name").isEmpty() ||
      param.value("transponder_sn").isEmpty() ||
      param.value("transponder_pan").isEmpty()) {
    sendLog(QString("Получены некорректные параметры. Сброс."));
    return ReturnStatus::ParameterError;
  }

  if ((param.value("issuer_name") != "Новое качество дорог") &&
      (param.value("issuer_name") == "Магистраль северной столицы")) {
    sendLog("Получено неизвестное название компании-эмитента. Сброс.");
    return ReturnStatus::ParameterError;
  }

  // Сохраняем данные стикера
  LastTransponderSticker = param;

  if (!initConnection()) {
    return ReturnStatus::StickerPrinterConnectionError;
  }

  if (param.value("issuer_name") == "Новое качество дорог") {
    printNkdSticker(param);
  } else if (param.value("issuer_name") == "Магистраль северной столицы") {
    printMssSticker(param);
  }

  closePort();
  return ReturnStatus::NoError;
}

ReturnStatus TE310Printer::printLastTransponderSticker() {
  if (LastPalletSticker.isEmpty()) {
    sendLog(
        "Ранее не было распечатано ни одного стикера на транспондер. Повтор "
        "печати невозможен.");
    return ReturnStatus::LastStickerMissed;
  }

  return printTransponderSticker(LastTransponderSticker);
}

ReturnStatus TE310Printer::printBoxSticker(const StringDictionary& param) {
  sendLog(QString("Печать стикера для бокса."));

  if (param.value("box_id").isEmpty() ||
      param.value("transponder_model").isEmpty() ||
      param.value("box_assembled_units").isEmpty() ||
      param.value("first_transponder_sn").isEmpty() ||
      param.value("last_transponder_sn").isEmpty()) {
    sendLog(QString("Получены некорректные параметры. Сброс."));
    return ReturnStatus::ParameterError;
  }

  // Сохраняем данные о стикере
  LastBoxSticker = param;

  if (!initConnection()) {
    return ReturnStatus::StickerPrinterConnectionError;
  }

  /* Командный скрипт
    SIZE 100 mm, 50 mm
    GAP 2 mm,2 mm
    REFERENCE 0,0
    DIRECTION 1
    CLS
    TEXT 600,10,"D.FNT",0,2,2,2,"JSC PowerSyntez"
    BOX 25, 50, 1150, 560, 4
    TEXT 50, 90, "D.FNT", 0, 2, 2, 1,"MODEL:"
    TEXT 600, 90, "D.FNT", 0, 2, 2, 1, "%1"
    TEXT 50, 190, "D.FNT", 0, 2, 2, 1, "QUANTITY:"
    TEXT 600, 190, "D.FNT", 0, 2, 2, 1, "%1"
    TEXT 50, 290, "D.FNT", 0, 2, 2, 1, "SERIAL NO FROM:"
    BARCODE 600, 275, "128M", 50, 2, 0, 2, 4, 1, "%1"
    TEXT 50, 390, "D.FNT", 0, 2, 2, 1, "SERIAL NO TO:"
    BARCODE 600, 375, "128M", 50, 2, 0, 2, 4, 1, "%1"
    TEXT 50, 490, "D.FNT", 0, 2, 2, 1, "BOX NO:"
    BARCODE 600, 475, "128M", 50, 2, 0, 2, 4, 1, "%1"
    PRINT 1
   */

  sendCommand("SIZE 100 mm, 50 mm");
  sendCommand("GAP 2 mm,2 mm");
  sendCommand("REFERENCE 0,0");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand("TEXT 600,10,\"D.FNT\",0,2,2,2,\"JSC PowerSyntez\"");
  sendCommand("BOX 25, 50, 1150, 560, 4 ");
  sendCommand("TEXT 50, 90, \"D.FNT\", 0, 2, 2, 1,\"MODEL:\"");
  sendCommand(QString("TEXT 600, 90, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("transponder_model"))
                  .toUtf8()
                  .data());
  /* На случай если модель будет заменена на артикул */
  //  sendCommand("TEXT 50, 90, \"D.FNT\", 0, 2, 2, 1,\"ARTICLE NO:\"");
  //  sendCommand(QString("BARCODE 600, 75, \"128M\", 50, 2, 0, 2, 4, 1,
  //  \"%1\"")
  //                  .arg(param.value("transponder_model"))
  //                  .toUtf8()
  //                  .data());
  sendCommand("TEXT 50, 190, \"D.FNT\", 0, 2, 2, 1, \"QUANTITY:\"");
  sendCommand(QString("TEXT 600, 190, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("box_assembled_units"))
                  .toUtf8()
                  .data());
  sendCommand("TEXT 50, 290, \"D.FNT\", 0, 2, 2, 1, \"SERIAL NO FROM:\"");
  sendCommand(QString("BARCODE 600, 275, \"128M\", 50, 2, 0, 2, 4, 1, \"%1\"")
                  .arg(param.value("first_transponder_sn"))
                  .toUtf8()
                  .data());
  sendCommand("TEXT 50, 390, \"D.FNT\", 0, 2, 2, 1, \"SERIAL NO TO:\"");
  sendCommand(QString("BARCODE 600, 375, \"128M\", 50, 2, 0, 2, 4, 1, \"%1\"")
                  .arg(param.value("last_transponder_sn"))
                  .toUtf8()
                  .data());
  sendCommand("TEXT 50, 490, \"D.FNT\", 0, 2, 2, 1, \"BOX NO:\"");
  sendCommand(QString("BARCODE 600, 475, \"128M\", 50, 2, 0, 2, 4, 1, \"%1\"")
                  .arg(param.value("box_id"))
                  .toUtf8()
                  .data());
  sendCommand("PRINT 1");

  closePort();

  return ReturnStatus::NoError;
}

ReturnStatus TE310Printer::printLastBoxSticker() {
  if (LastBoxSticker.isEmpty()) {
    sendLog(
        "Ранее не было распечатано ни одного стикера на бокс. Повтор печати "
        "невозможен.");
    return ReturnStatus::LastStickerMissed;
  }

  return printBoxSticker(LastBoxSticker);
}

ReturnStatus TE310Printer::printPalletSticker(const StringDictionary& param) {
  sendLog(QString("Печать стикера для бокса."));

  if (param.value("box_id").isEmpty() ||
      param.value("transponder_model").isEmpty() ||
      param.value("box_assembled_units").isEmpty() ||
      param.value("first_transponder_sn").isEmpty() ||
      param.value("last_transponder_sn").isEmpty() ||
      param.value("employee_data").isEmpty()) {
    sendLog(QString("Получены некорректные параметры. Сброс."));
    return ReturnStatus::ParameterError;
  }

  // Сохраняем данные о стикере
  LastBoxSticker = param;

  if (!initConnection()) {
    return ReturnStatus::StickerPrinterConnectionError;
  }

  /* Командный скрипт:
    SIZE 100 mm, 50 mm
    GAP 2 mm,2 mm
    REFERENCE 0,0
    DIRECTION 1
    CLS

   TEXT 600,10,"D.FNT",0,2,2,2,"JSC PowerSyntez"
   BOX 25, 50, 1150, 525, 4

   TEXT 50, 75, "D.FNT", 0, 2, 2, 1,"MODEL:"
   TEXT 600, 75, "D.FNT", 0, 2, 2, 1, "TC1001"

   TEXT 50, 150, "D.FNT", 0, 2, 2, 1, "QUANTITY:"
   TEXT 600, 150, "D.FNT", 0, 2, 2, 1, "50"

   TEXT 50, 225, "D.FNT", 0, 2, 2, 1, "SERIAL NO FROM:"
   TEXT 600, 225, "D.FNT", 0, 2, 2, 1, "501234NNNNNNNNNN"

   TEXT 50, 300, "D.FNT", 0, 2, 2, 1, "SERIAL NO TO:"
   TEXT 600, 300, "D.FNT", 0, 2, 2, 1, "501234NNNNNNNNNN"

   TEXT 50, 375, "D.FNT", 0, 2, 2, 1, "BOX NO:"
   TEXT 600, 375, "D.FNT", 0, 2, 2, 1, "1000555"

   TEXT 50, 450, "D.FNT", 0, 2, 2, 1, "EMPLOYEE:"
   TEXT 600, 450, "D.FNT", 0, 2, 2, 1, "NAME SURNAME"

   TEXT 600, 535, "D.FNT", 0, 2, 2, 2,"Made in Russia"

   PRINT 1
  */

  sendCommand("SIZE 100 mm, 50 mm");
  sendCommand("GAP 2 mm,2 mm");
  sendCommand("REFERENCE 0,0");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand("TEXT 600,10,\"D.FNT\",0,2,2,2,\"JSC PowerSyntez\"");
  sendCommand("BOX 25, 50, 1150, 525, 4 ");

  sendCommand("TEXT 50, 75, \"D.FNT\", 0, 2, 2, 1,\"MODEL:\"");
  sendCommand(QString("TEXT 600, 75, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("transponder_model"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 50, 150, \"D.FNT\", 0, 2, 2, 1, \"QUANTITY:\"");
  sendCommand(QString("TEXT 600, 150, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("box_assembled_units"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 50, 225, \"D.FNT\", 0, 2, 2, 1, \"SERIAL NO FROM:\"");
  sendCommand(QString("TEXT 600, 225, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("first_transponder_sn"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 50, 300, \"D.FNT\", 0, 2, 2, 1, \"SERIAL NO TO:\"");
  sendCommand(QString("TEXT 600, 300, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("last_transponder_sn"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 50, 375, \"D.FNT\", 0, 2, 2, 1, \"BOX NO:\"");
  sendCommand(QString("TEXT 600, 375, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("box_id"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 50, 450, \"D.FNT\", 0, 2, 2, 1, \"EMPLOYEE:\"");
  sendCommand(QString("TEXT 600, 450, \"D.FNT\", 0, 2, 2, 1, \"%1\"")
                  .arg(param.value("employee_data"))
                  .toUtf8()
                  .data());

  sendCommand("TEXT 600, 535, \"D.FNT\", 0, 2, 2, 2,\"Made in Russia\"");
  sendCommand("PRINT 1");

  closePort();

  return ReturnStatus::NoError;
}

ReturnStatus TE310Printer::printLastPalletSticker() {
  if (LastPalletSticker.isEmpty()) {
    sendLog(
        "Ранее не было распечатано ни одного стикера на паллету. Повтор печати "
        "невозможен.");
    return ReturnStatus::LastStickerMissed;
  }

  return printPalletSticker(LastPalletSticker);
}

ReturnStatus TE310Printer::exec(const QStringList& commandScript) {
  if (!initConnection()) {
    return ReturnStatus::StickerPrinterConnectionError;
  }

  bool ok = true;
  for (int32_t i = 0; i < commandScript.size() && ok; i++) {
    //    QByteArray encodedData = commandScript.at(i).toUtf8();
    //    const char* charData = encodedData.constData();

    ok = sendCommand(commandScript.at(i).toUtf8().constData());
  }

  closePort();

  if (!ok) {
    sendLog("Получена ошибка при выполнении командного скрипта.");
    return ReturnStatus::StickerPrinterCommandScriptExecutionError;
  }

  sendLog("Выполнение командного скрипта успешно завершено.");
  return ReturnStatus::NoError;
}

void TE310Printer::loadSettings() {
  sendLog("Загрузка настроек.");
  doLoadSettings();
  loadTscLib();
}

void TE310Printer::doLoadSettings() {
  QSettings settings;

  TscLibPath =
      settings.value(QString("%1/library_path").arg(objectName())).toString();
  SystemName =
      settings.value(QString("%1/system_name").arg(objectName())).toString();

  UseEthernet =
      settings.value(QString("%1/use_ethernet").arg(objectName())).toBool();
  if (UseEthernet) {
    IPAddress = QHostAddress(
        settings.value(QString("%1/ip").arg(objectName())).toString());
    Port = settings.value(QString("%1/port").arg(objectName())).toInt();
  }
}

void TE310Printer::loadTscLib() {
  TscLib = std::unique_ptr<QLibrary>(new QLibrary(TscLibPath));

  if (!TscLib->load()) {
    about = nullptr;
    openPort = nullptr;
    openEthernet = nullptr;
    sendCommand = nullptr;
    closePort = nullptr;
  }
  about = (TscAbout)TscLib->resolve("about");
  openPort = (TscOpenPort)TscLib->resolve("openport");
  openEthernet = (TscOpenEthernet)TscLib->resolve("openethernet");
  sendCommand = (TscSendCommand)TscLib->resolve("sendcommand");
  closePort = (TscClosePort)TscLib->resolve("closeport");
}

bool TE310Printer::initConnection() {
  if (!UseEthernet) {
    if (openPort(SystemName.toUtf8().constData()) == 0) {
      sendLog("Не удалось установить соединение.");
      return false;
    }
  } else {
    int32_t result = openEthernet(IPAddress.toString().toUtf8().data(), Port);
    if (result == 0) {
      sendLog("Не удалось установить соединение.");
      return false;
    }
  }

  sendLog("Соединение установлено. ");
  return true;
}

void TE310Printer::printNkdSticker(const StringDictionary& param) {
  sendCommand("SIZE 27 mm, 27 mm");
  sendCommand("GAP 2 mm,2 mm");
  sendCommand("REFERENCE 0,0");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand(QString("TEXT 162,30,\"D.FNT\",0,1,1,2,\"PAN: %1\"")
                  .arg(param.value("transponder_pan"))
                  .toUtf8()
                  .data());
  sendCommand(
      QString("QRCODE "
              "60,60,H,10,A,0,X204,J1,M2,\"%1\n\r%2\"")
          .arg(param.value("transponder_pan"), param.value("transponder_sn"))
          .toUtf8()
          .data());
  sendCommand(QString("TEXT 162,276,\"D.FNT\",0,1,1,2,\"SN: %1\"")
                  .arg(param.value("transponder_sn"))
                  .toUtf8()
                  .data());
  sendCommand("PRINT 1");
}

void TE310Printer::printMssSticker(const StringDictionary& param) {
  /* Командный скрипт:
    SIZE 30 mm, 20 mm
    GAP 2 mm, 1 mm
    DIRECTION 1
    CLS
    TEXT 180,12,"D.FNT",0,1,1,2,"SN: 5012341234567890"
    BARCODE 18,36,"128",144,2,0,2,2,"1234567890123456789"
    PRINT 1
  */

  sendCommand("SIZE 30 mm, 20 mm");
  sendCommand("GAP 2 mm, 1 mm");
  sendCommand("DIRECTION 1");
  sendCommand("CLS");
  sendCommand(QString("TEXT 180,12,\"D.FNT\",0,1,1,2,\"SN: %1\"")
                  .arg(param.value("transponder_sn"))
                  .toUtf8()
                  .data());
  sendCommand(QString("BARCODE 18,36,\"128\",144,2,0,2,2,\"%1\"")
                  .arg(param.value("transponder_pan"))
                  .toUtf8()
                  .data());
  sendCommand("PRINT 1");
}
