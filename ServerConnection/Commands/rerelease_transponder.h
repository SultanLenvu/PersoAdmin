#ifndef RERELEASETRANSPONDERCOMMAND_H
#define RERELEASETRANSPONDERCOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class RereleaseTransponder : public AbstractClientCommand {
  Q_OBJECT

 private:
  const QString Name = COMMAND_RERELEASETRANSPONDER_NAME;
  const size_t ResponseSize = COMMAND_RERELEASETRANSPONDER_RESPONSE_SIZE;

 public:
  explicit RereleaseTransponder(const QString& name);
  ~RereleaseTransponder();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(RereleaseTransponder)

 signals:
};

#endif  // RERELEASETRANSPONDERCOMMAND_H
