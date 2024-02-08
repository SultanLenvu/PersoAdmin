#ifndef REFUNDCURRENTBOXDATA_H
#define REFUNDCURRENTBOXDATA_H

#include "abstract_client_command.h"
#include "definitions.h"

class RefundCurrentBox : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_REFUNDCURRENTBOX_NAME;
  const size_t ResponseSize = COMMAND_REFUNDBOX_RESPONSE_SIZE;

 public:
  explicit RefundCurrentBox(const QString& name);
  ~RefundCurrentBox();

 private:
  Q_DISABLE_COPY_MOVE(RefundCurrentBox)

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  bool checkSyntax(void);

 signals:
};

#endif  // REFUNDCURRENTBOXDATA_H
