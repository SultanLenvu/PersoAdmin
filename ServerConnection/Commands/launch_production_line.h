#ifndef LOGIN_H
#define LOGIN_H

#include "abstract_client_command.h"
#include "definitions.h"

class LaunchProductionLine : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_LAUNCHPRODUCTIONLINE_NAME;
  const size_t ResponseSize = COMMAND_LAUNCHPRODUCTIONLINE_RESPONSE_SIZE;

 public:
  explicit LaunchProductionLine(const QString& name);
  ~LaunchProductionLine();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(LaunchProductionLine)

 signals:
};

#endif  // LOGIN_H
