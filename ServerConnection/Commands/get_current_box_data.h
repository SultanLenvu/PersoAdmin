#ifndef GETCURRENTBOXDATA_H
#define GETCURRENTBOXDATA_H

#include "abstract_client_command.h"
#include "definitions.h"

class GetCurrentBoxData : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_GETCURRENTBOXDATA_NAME;
  const size_t ResponseSize = COMMAND_GETCURRENTBOXDATA_RESPONSE_SIZE;

 public:
  explicit GetCurrentBoxData(const QString& name);
  ~GetCurrentBoxData();

 private:
  Q_DISABLE_COPY_MOVE(GetCurrentBoxData)

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

#endif  // GETCURRENTBOXDATA_H
