#ifndef STATUSINDICATOR_H
#define STATUSINDICATOR_H

#include "i_status_indicator.h"
#include "named_object.h"
#include "types.h"

class StatusIndicator final : public NamedObject, public IStatusIndicator {
  Q_OBJECT

 private:
  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  StatusIndicator(const QString& name);
  ~StatusIndicator() = default;

  // IStatusIndicator interface
 public:
  virtual void handle(ReturnStatus ret) override;

 private:
  void createMessageMatchTable(void);
};

#endif  // STATUSINDICATOR_H
