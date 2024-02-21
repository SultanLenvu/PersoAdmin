#ifndef ISTICKERPRINTER_H
#define ISTICKERPRINTER_H

#include <QHash>
#include <QLibrary>
#include <QObject>
#include <QSettings>
#include <QtPrintSupport/QPrinterInfo>

#include "pobject.h"
#include "types.h"

class AbstractStickerPrinter : public PObject {
  Q_OBJECT
 public:
  AbstractStickerPrinter(const QString& name);
  virtual ~AbstractStickerPrinter();

  // PObject interface
 public slots:
  virtual void applySetting(void) = 0;

  // Own
 public slots:
  virtual ReturnStatus checkConfig(void) = 0;

  virtual ReturnStatus printTransponderSticker(
      const StringDictionary& param) = 0;
  virtual ReturnStatus printLastTransponderSticker(void) = 0;

  virtual ReturnStatus printBoxSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastBoxSticker(void) = 0;

  virtual ReturnStatus printPalletSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastPalletSticker(void) = 0;

  virtual ReturnStatus exec(const QStringList& commandScript) = 0;

 signals:
};

#endif  // ISTICKERPRINTER_H
