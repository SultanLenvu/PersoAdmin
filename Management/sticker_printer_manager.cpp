#include "sticker_printer_manager.h"

StickerPrinterManager::StickerPrinterManager(const QString& name)
    : AbstractManager(name) {}

StickerPrinterManager::~StickerPrinterManager() {}

AbstractManager::Type StickerPrinterManager::type() const {
  return StickerPrinter;
}

void StickerPrinterManager::onInstanceThreadStarted() {}

void StickerPrinterManager::applySettings() {}

void StickerPrinterManager::printTransponderSticker(
    const std::shared_ptr<StringDictionary> param) {}

void StickerPrinterManager::printBoxSticker(
    const std::shared_ptr<StringDictionary> param) {}

void StickerPrinterManager::printPalletSticker(
    const std::shared_ptr<StringDictionary> param) {}

void StickerPrinterManager::execCommandScript(
    const std::shared_ptr<QStringList> script) {}
