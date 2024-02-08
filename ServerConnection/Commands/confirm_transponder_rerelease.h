#ifndef RERELEASECONFIRMCOMMAND_H
#define RERELEASECONFIRMCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class ConfirmTransponderRerelease : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_CONFIRMTRANSPONDERRERELEASE_NAME;
  const size_t ResponseSize = COMMAND_CONFIRMTRANSPONDERRERELEASE_RESPONSE_SIZE;

 public:
  explicit ConfirmTransponderRerelease(const QString& name);
  ~ConfirmTransponderRerelease();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(ConfirmTransponderRerelease)

 signals:
};

#endif  // RERELEASECONFIRMCOMMAND_H
