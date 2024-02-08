#ifndef GETCURRENTTRANSPONDERDATA_H
#define GETCURRENTTRANSPONDERDATA_H

#include "abstract_client_command.h"
#include "definitions.h"

class GetCurrentTransponderData : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_GETCURRENTTRANSPONDERDATA_NAME;
  const size_t ResponseSize = COMMAND_GETCURRENTTRANSPONDERDATA_RESPONSE_SIZE;

 public:
  explicit GetCurrentTransponderData(const QString& name);
  ~GetCurrentTransponderData();

 private:
  Q_DISABLE_COPY_MOVE(GetCurrentTransponderData)

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

#endif  // GETCURRENTTRANSPONDERDATA_H
