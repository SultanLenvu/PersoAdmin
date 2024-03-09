#ifndef RETURNSTATUSHANDLER_H
#define RETURNSTATUSHANDLER_H

#include "abstract_return_status_handler.h"
#include "named_object.h"
#include "types.h"

class ReturnStatusHandler final : public NamedObject,
                                  public AbstractReturnStatusHandler {
  Q_OBJECT

 private:
  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  ReturnStatusHandler(const QString& name);
  ~ReturnStatusHandler();

  // AbstractReturnStatusHandler interface
 public:
  virtual void handle(ReturnStatus ret) override;

 private:
  void createMessageMatchTable(void);
};

#endif // RETURNSTATUSHANDLER_H
