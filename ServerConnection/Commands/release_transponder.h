#ifndef RELEASECOMMAND_H
#define RELEASECOMMAND_H

#include "abstract_client_command.h"
#include "definitions.h"

class ReleaseTransponder : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_RELEASETRANSPONDER_NAME;
  const size_t ResponseSize = COMMAND_RELEASETRANSPONDER_RESPONSE_SIZE;

 public:
  explicit ReleaseTransponder(const QString& name);
  ~ReleaseTransponder();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(ReleaseTransponder)
  bool checkSyntax(void);

 signals:
};

#endif  // RELEASECOMMAND_H
