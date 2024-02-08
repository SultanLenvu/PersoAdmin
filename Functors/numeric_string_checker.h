#ifndef NUMERICSTRINGCHECKER_H
#define NUMERICSTRINGCHECKER_H

#include "abstract_string_checker.h"

class NumericStringChecker final : public AbstractStringChecker {
 public:
  NumericStringChecker();
  ~NumericStringChecker();

 public:
  virtual bool operator()(const std::string&) const override;
};

#endif // NUMERICSTRINGCHECKER_H
