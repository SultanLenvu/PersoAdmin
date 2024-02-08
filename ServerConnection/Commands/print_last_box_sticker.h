#ifndef PRINTLASTBOXSTICKER_H
#define PRINTLASTBOXSTICKER_H

#include "abstract_client_command.h"
#include "definitions.h"

class PrintLastBoxSticker : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_PRINTLASTBOXSTICKER_NAME;
  const size_t ResponseSize = COMMAND_PRINTLASTBOXSTICKER_RESPONSE_SIZE;

 public:
  explicit PrintLastBoxSticker(const QString& name);
  ~PrintLastBoxSticker();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(PrintLastBoxSticker)

 signals:
  void printLastBoxSticker_signal(ReturnStatus& status);
};

#endif  // PRINTLASTBOXSTICKER_H
