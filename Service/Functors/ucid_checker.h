#ifndef UCIDCHECKER_H
#define UCIDCHECKER_H

#include "abstract_string_checker.h"

class UcidChecker final : public AbstractStringChecker {
 public:
  UcidChecker() = default;
  ~UcidChecker() = default;

 public:
  virtual bool operator()(const QString&) const override;

 private:
  Q_DISABLE_COPY_MOVE(UcidChecker)
};

#endif // UCIDCHECKER_H
