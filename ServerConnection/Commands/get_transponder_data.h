#ifndef GETTRANSPONDERDATA_H
#define GETTRANSPONDERDATA_H

#include "abstract_client_command.h"
#include "definitions.h"

class GetTransponderData : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_GETTRANSPONDERDATA_NAME;
  const size_t ResponseSize = COMMAND_GETCURRENTTRANSPONDERDATA_RESPONSE_SIZE;

 public:
  explicit GetTransponderData(const QString& name);
  ~GetTransponderData();

 private:
  Q_DISABLE_COPY_MOVE(GetTransponderData)

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
