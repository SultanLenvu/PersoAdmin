#ifndef TE310PRINTER_H
#define TE310PRINTER_H

#include <QHostAddress>
#include <QHostInfo>
#include <QLibrary>

#include "i_sticker_printer.h"
#include "configurable_object.h"
#include "loggable_object.h"
#include "named_object.h"

class TE310Printer final : public NamedObject,
                           public IStickerPrinter,
                           public ConfigurableObject,
                           public LoggableObject {
  Q_OBJECT

 private:
  // Прототипы библиотечных функции
  typedef int (*TscAbout)(void);
  typedef int (*TscOpenPort)(const char*);
  typedef int (*TscSendCommand)(const char*);
  typedef int (*TscClosePort)(void);
  typedef int (*TscOpenEthernet)(const char*, int);

 private:
  QString SystemName;

  bool UseEthernet;
  QHostAddress IPAddress;
  int32_t Port;

  QString TscLibPath;
  std::unique_ptr<QLibrary> TscLib;

  StringDictionary LastTransponderSticker;
  StringDictionary LastBoxSticker;
  StringDictionary LastPalletSticker;

  // Библиотечные функции
  TscAbout about;
  TscOpenPort openPort;
  TscSendCommand sendCommand;
  TscClosePort closePort;
  TscOpenEthernet openEthernet;

 public:
  explicit TE310Printer(const QString& name);
  ~TE310Printer() = default;

  // IStickerPrinter interface
 public slots:
  virtual ReturnStatus checkConfig(void) override;

  virtual ReturnStatus printTransponderSticker(
      const StringDictionary& param) override;
  virtual ReturnStatus printLastTransponderSticker(void) override;

  virtual ReturnStatus printBoxSticker(const StringDictionary& param) override;
  virtual ReturnStatus printLastBoxSticker(void) override;

  virtual ReturnStatus printPalletSticker(
      const StringDictionary& param) override;
  virtual ReturnStatus printLastPalletSticker(void) override;

  virtual ReturnStatus exec(const QStringList& commandScript) override;

 private:
  Q_DISABLE_COPY_MOVE(TE310Printer);

 private:
  virtual void loadSettings(void) override;
  void doLoadSettings(void);
  void loadTscLib(void);

 private:
  bool initConnection(void);
  void printNkdSticker(const StringDictionary& param);
  void printMssSticker(const StringDictionary& param);
};

#endif  // TE310PRINTER_H
