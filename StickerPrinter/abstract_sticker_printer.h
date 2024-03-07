#ifndef ISTICKERPRINTER_H
#define ISTICKERPRINTER_H

#include <QHash>
#include <QLibrary>
#include <QObject>
#include <QSettings>
#include <QtPrintSupport/QPrinterInfo>

#include "types.h"

class AbstractStickerPrinter {
 public:
  AbstractStickerPrinter() = default;
  virtual ~AbstractStickerPrinter() = default;

 public:
  virtual ReturnStatus checkConfig(void) = 0;

  virtual ReturnStatus printTransponderSticker(
      const StringDictionary& param) = 0;
  virtual ReturnStatus printLastTransponderSticker(void) = 0;

  virtual ReturnStatus printBoxSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastBoxSticker(void) = 0;

  virtual ReturnStatus printPalletSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastPalletSticker(void) = 0;

  virtual ReturnStatus exec(const QStringList& commandScript) = 0;
};

#endif  // ISTICKERPRINTER_H
