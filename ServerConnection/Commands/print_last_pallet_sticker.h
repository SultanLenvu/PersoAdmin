#ifndef LASTPALLETSTICKERPRINTCOMMAND_H
#define LASTPALLETSTICKERPRINTCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class PrintLastPalletSticker : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_PRINTLASTPALLETSTICKER_NAME;
  const size_t ResponseSize = COMMAND_PRINTLASTPALLETSTICKER_RESPONSE_SIZE;

 public:
  explicit PrintLastPalletSticker(const QString& name);
  ~PrintLastPalletSticker();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(PrintLastPalletSticker)

 signals:
};

#endif  // LASTPALLETSTICKERPRINTCOMMAND_H
