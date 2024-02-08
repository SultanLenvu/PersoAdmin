#ifndef RELEASECONFIRMCOMMAND_H
#define RELEASECONFIRMCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class ConfirmTransponderRelease : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_CONFIRMTRANSPONDERRELEASE_NAME;
  const size_t ResponseSize = COMMAND_CONFIRMTRANSPONDERRELEASE_RESPONSE_SIZE;

 public:
  explicit ConfirmTransponderRelease(const QString& name);
  ~ConfirmTransponderRelease();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(ConfirmTransponderRelease)

 signals:
};

#endif  // RELEASECONFIRMCOMMAND_H
