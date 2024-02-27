#ifndef RETURNSTATUSHANDLER_H
#define RETURNSTATUSHANDLER_H

#include "abstract_return_status_handler.h"
#include "types.h"

class GuiReturnStatusHandler final : public AbstractReturnStatusHandler {
  Q_OBJECT

 private:
  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  GuiReturnStatusHandler(const QString& name);
  ~GuiReturnStatusHandler();

  // AbstractReturnStatusHandler interface
 public:
  virtual void handle(ReturnStatus ret) override;

 private:
  void createMessageMatchTable(void);
};

#endif // RETURNSTATUSHANDLER_H
