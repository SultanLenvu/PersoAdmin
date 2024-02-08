#ifndef COMPLETECURRENTBOX_H
#define COMPLETECURRENTBOX_H

#include "abstract_client_command.h"
#include "definitions.h"

class CompleteCurrentBox : public AbstractClientCommand {
  Q_OBJECT
 private:
  const QString Name = COMMAND_COMPLETECURRENTBOX_NAME;
  const size_t ResponseSize = COMMAND_COMPLETECURRENTBOX_RESPONSE_SIZE;

 public:
  explicit CompleteCurrentBox(const QString& name);
  ~CompleteCurrentBox();

  // AbstractClientCommand interface
 public:
  virtual const QString& name() override;
  virtual ReturnStatus generate(const StringDictionary& param,
                                QByteArray& dataBlock) override;
  virtual ReturnStatus processResponse(const QByteArray& dataBlock,
                                       StringDictionary& responseData) override;

 private:
  Q_DISABLE_COPY_MOVE(CompleteCurrentBox)

 signals:
};

#endif  // COMPLETECURRENTBOX_H
