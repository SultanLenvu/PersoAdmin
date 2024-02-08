#ifndef ROLLBACKCOMMAND_H
#define ROLLBACKCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class RollbackTransponder : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_ROLLBACKTRANSPONDER_NAME;
  const size_t ResponseSize = COMMAND_ROLLBACKTRANSPONDER_RESPONSE_SIZE;

 public:
  explicit RollbackTransponder(const QString& name);
  ~RollbackTransponder();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(RollbackTransponder)

 signals:
};

#endif  // ROLLBACKCOMMAND_H
