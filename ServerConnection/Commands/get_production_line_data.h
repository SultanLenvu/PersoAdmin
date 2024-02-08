#ifndef GETPRODUCTIONLINEDATA_H
#define GETPRODUCTIONLINEDATA_H

#include "abstract_client_command.h"
#include "definitions.h"

class GetProductionLineData : public AbstractClientCommand
{
  Q_OBJECT
 private:
  const QString Name = COMMAND_GETPRODUCTIONLINEDATA_NAME;
  const size_t ResponseSize = COMMAND_GETNPRODUCTIONLINEDATA_RESPONSE_SIZE;

 public:
  explicit GetProductionLineData(const QString& name);
  ~GetProductionLineData();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(GetProductionLineData)
  bool checkSyntax(void);

 signals:
};

#endif // GETPRODUCTIONLINEDATA_H
