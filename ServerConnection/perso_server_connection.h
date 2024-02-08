#ifndef PERSO_SERVER_CONNECTION_H
#define PERSO_SERVER_CONNECTION_H

#include <QCoreApplication>
#include <QDataStream>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include "abstract_client_command.h"
#include "abstract_server_connection.h"

class PersoServerConnection : public AbstractServerConnection {
  Q_OBJECT
 public:
  enum CommandId {
    Echo,
    LaunchProductionLine,
    ShutdownProductionLine,
    GetProductionLineData,

    RequestBox,
    GetCurrentBoxData,
    CompleteCurrentBox,
    RefundCurrentBox,

    GetCurrentTransponderData,
    GetTransponderData,

    ReleaseTransponder,
    ConfirmTransponderRelease,
    RereleaseTransponder,
    ConfirmTransponderRerelease,
    RollbackTransponder,

    PrintBoxSticker,
    PrintLastBoxSticker,
    PrintPalletSticker,
    PrintLastPalletSticker,
  };
  Q_ENUM(CommandId)

 private:
  QHostAddress PersoServerAddress;
  uint32_t PersoServerPort;

  std::unique_ptr<QTcpSocket> Socket;

  std::shared_ptr<AbstractClientCommand> CurrentCommand;
  std::unordered_map<CommandId, std::shared_ptr<AbstractClientCommand>>
      Commands;

  uint32_t ReceivedDataBlockSize;
  QByteArray ReceivedDataBlock;

  std::unique_ptr<QTimer> WaitTimer;
  std::unique_ptr<QEventLoop> WaitingLoop;

 public:
  explicit PersoServerConnection(const QString& name);
  ~PersoServerConnection();

  // AbstractServerConnection interface
 public:
  virtual ReturnStatus connect() override;
  virtual void disconnect() override;
  virtual bool isConnected(void) override;
  virtual ReturnStatus echo() override;

  virtual ReturnStatus launchProductionLine(const StringDictionary& param) override;
  virtual ReturnStatus shutdownProductionLine() override;
  virtual ReturnStatus getProductionLineData(StringDictionary& data) override;

  virtual ReturnStatus requestBox(void) override;
  virtual ReturnStatus getCurrentBoxData(StringDictionary& result) override;
  virtual ReturnStatus completeCurrentBox(void) override;
  virtual ReturnStatus refundCurrentBox(void) override;

  virtual ReturnStatus getCurrentTransponderData(
      StringDictionary& result) override;
  virtual ReturnStatus getTransponderData(const StringDictionary& param,
                                          StringDictionary& result) override;

  virtual ReturnStatus releaseTransponder(StringDictionary& result) override;
  virtual ReturnStatus confirmTransponderRelease(
      const StringDictionary& param) override;
  virtual ReturnStatus rereleaseTransponder(const StringDictionary& param,
                                            StringDictionary& result) override;
  virtual ReturnStatus confirmTransponderRerelease(
      const StringDictionary& param) override;
  virtual ReturnStatus rollbackTransponder(void) override;

  virtual ReturnStatus printBoxSticker(const StringDictionary& param) override;
  virtual ReturnStatus printLastBoxSticker(void) override;
  virtual ReturnStatus printPalletSticker(
      const StringDictionary& param) override;
  virtual ReturnStatus printLastPalletSticker(void) override;

  virtual void applySettings(void) override;

 private:
  Q_DISABLE_COPY_MOVE(PersoServerConnection)
  void loadSettings(void);
  void sendLog(const QString& log);

  ReturnStatus processCurrentCommand(const StringDictionary& param,
                                     StringDictionary& result);
  ReturnStatus transmitDataBlock(const QByteArray& dataBlock);
  bool waitResponse(void);

  // Фабричные методы
  void createTimers(void);
  void createSocket(void);
  void createCommands(void);

  void clearBuffer(void);

 private slots:
  void socketConnected_slot(void);
  void socketDisconnected_slot(void);

  void socketReadyRead_slot(void);
  void socketError_slot(QAbstractSocket::SocketError socketError);

  void waitTimerTimeout_slot(void);

 signals:
  void stopResponseWaiting(void);
};

#endif  // PERSO_SERVER_CONNECTION_H
