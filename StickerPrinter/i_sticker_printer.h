#ifndef ISTICKERPRINTER_H
#define ISTICKERPRINTER_H

#include "types.h"

class IStickerPrinter {
 public:
  IStickerPrinter() = default;
  virtual ~IStickerPrinter() = default;

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
