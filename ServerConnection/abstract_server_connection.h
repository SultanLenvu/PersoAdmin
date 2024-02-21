#ifndef ABSTARCTSERVERCONNECTION_H
#define ABSTARCTSERVERCONNECTION_H

#include "psobject.h"
#include "types.h"

class AbstractServerConnection : public PObject {
  Q_OBJECT
 public:
  explicit AbstractServerConnection(const QString& name);
  virtual ~AbstractServerConnection();

  // Own
 public:
  virtual ReturnStatus connect(void) = 0;
  virtual void disconnect(void) = 0;
  virtual bool isConnected(void) = 0;

  virtual ReturnStatus echo(void) = 0;

  virtual ReturnStatus launchProductionLine(const StringDictionary& param) = 0;
  virtual ReturnStatus shutdownProductionLine(void) = 0;
  virtual ReturnStatus getProductionLineData(StringDictionary& result) = 0;

  virtual ReturnStatus requestBox(void) = 0;
  virtual ReturnStatus getCurrentBoxData(StringDictionary& result) = 0;
  virtual ReturnStatus completeCurrentBox(void) = 0;
  virtual ReturnStatus refundCurrentBox(void) = 0;

  virtual ReturnStatus getCurrentTransponderData(StringDictionary& result) = 0;
  virtual ReturnStatus getTransponderData(const StringDictionary& param,
                                          StringDictionary& result) = 0;

  virtual ReturnStatus releaseTransponder(StringDictionary& result) = 0;
  virtual ReturnStatus confirmTransponderRelease(
      const StringDictionary& param) = 0;
  virtual ReturnStatus rereleaseTransponder(const StringDictionary& param,
                                            StringDictionary& result) = 0;
  virtual ReturnStatus confirmTransponderRerelease(
      const StringDictionary& param) = 0;
  virtual ReturnStatus rollbackTransponder(void) = 0;

  virtual ReturnStatus printBoxSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastBoxSticker(void) = 0;
  virtual ReturnStatus printPalletSticker(const StringDictionary& param) = 0;
  virtual ReturnStatus printLastPalletSticker(void) = 0;

 private:
  void connectDepedencies(void);

 signals:
  void disconnected(void);
};

#endif  // ABSTARCTSERVERCONNECTION_H
