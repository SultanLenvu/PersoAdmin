#ifndef NUMERICSTRINGCHECKER_H
#define NUMERICSTRINGCHECKER_H

#include "abstract_string_checker.h"

class NumericStringChecker final : public AbstractStringChecker {
 public:
  explicit NumericStringChecker() = default;
  ~NumericStringChecker() = default;

 public:
  virtual bool operator()(const QString&) const override;

 private:
  Q_DISABLE_COPY_MOVE(NumericStringChecker)
};

#endif // NUMERICSTRINGCHECKER_H
