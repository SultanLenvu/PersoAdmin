#ifndef REQUESTBOX_H
#define REQUESTBOX_H

#include "abstract_client_command.h"
#include "definitions.h"

class RequestBox final : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_REQUESTBOX_NAME;
  const size_t ResponseSize = COMMAND_REQUESTBOX_RESPONSE_SIZE;

 public:
  explicit RequestBox(const QString& name);
  ~RequestBox() = default;

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(RequestBox)
};

#endif  // REQUESTBOX_H
