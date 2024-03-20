#ifndef LOGOUT_H
#define LOGOUT_H

#include "abstract_client_command.h"
#include "definitions.h"

class ShutdownProductionLine final : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_SHUTDOWNPRODUCTIONLINE_NAME;
  const size_t ResponseSize = COMMAND_SHUTDOWNPRODUCTIONLINE_RESPONSE_SIZE;

 public:
  explicit ShutdownProductionLine(const QString& name);
  ~ShutdownProductionLine() = default;

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(ShutdownProductionLine)
};

#endif  // LOGOUT_H
