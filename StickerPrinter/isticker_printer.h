#ifndef ISTICKERPRINTER_H
#define ISTICKERPRINTER_H

#include <QLibrary>
#include <QMap>
#include <QObject>
#include <QSettings>
#include <QtPrintSupport/QPrinterInfo>

class IStickerPrinter : public QObject {
  Q_OBJECT
 public:
  enum PrinterType {
    Unknown,
    TE310,
  };

  enum ReturnStatus {
    ParameterError,
    LibraryMissing,
    NotConnected,
    Failed,
    Completed,
  };

 protected:
  PrinterType Type;

 public:
  IStickerPrinter(QObject* parent, PrinterType type);

  virtual ReturnStatus printTransponderSticker(
      const QMap<QString, QString>* parameters) = 0;
  virtual ReturnStatus printLastTransponderSticker(void) = 0;
  virtual ReturnStatus printBoxSticker(
      const QMap<QString, QString>* parameters) = 0;
  virtual ReturnStatus printPalletSticker(
      const QMap<QString, QString>* parameters) = 0;

  virtual ReturnStatus exec(const QStringList* commandScript) = 0;

  virtual void applySetting(void) = 0;

 signals:
  void logging(const QString& log);
  void operationFinished(ReturnStatus ret);
};

#endif  // ISTICKERPRINTER_H
