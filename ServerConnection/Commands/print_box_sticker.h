#ifndef BOXSTICKERPRINTCOMMAND_H
#define BOXSTICKERPRINTCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class PrintBoxSticker : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_PRINTBOXSTICKER_NAME;
  const size_t ResponseSize = COMMAND_PRINTBOXSTICKER_RESPONSE_SIZE;

  StringDictionary Parameters;
  ReturnStatus Status;

 public:
  explicit PrintBoxSticker(const QString& name);
  ~PrintBoxSticker();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(PrintBoxSticker)

 signals:
};

#endif  // BOXSTICKERPRINTCOMMAND_H
