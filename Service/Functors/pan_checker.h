#ifndef PANCHECKER_H
#define PANCHECKER_H

#include "abstract_string_checker.h"

class PanChecker final : public AbstractStringChecker {
 public:
  explicit PanChecker() = default;
  ~PanChecker() = default;

 public:
  virtual bool operator()(const QString&) const override;

 private:
  Q_DISABLE_COPY_MOVE(PanChecker)
};

#endif // PANCHECKER_H
