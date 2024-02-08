#include "numeric_string_checker.h"

#include <regex>

NumericStringChecker::NumericStringChecker()
{
  
}

NumericStringChecker::~NumericStringChecker() {}

bool NumericStringChecker::operator()(const std::string& str) const {
  std::regex pattern("[0-9]+");
  return std::regex_match(str, pattern);
}
