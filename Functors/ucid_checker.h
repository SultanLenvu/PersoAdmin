#ifndef UCIDCHECKER_H
#define UCIDCHECKER_H

#include "abstract_string_checker.h"

class UcidChecker : public AbstractStringChecker {
 public:
  UcidChecker();
  ~UcidChecker();

 public:
  virtual bool operator()(const std::string&) const override;
};

#endif // UCIDCHECKER_H
