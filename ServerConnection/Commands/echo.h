#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class Echo : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_ECHO_NAME;
  const size_t ResponseSize = COMMAND_ECHO_RESPONSE_SIZE;

 public:
  explicit Echo(const QString& name);
  ~Echo();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(Echo)
};

#endif  // ECHOCOMMAND_H
