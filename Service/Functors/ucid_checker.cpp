#include "ucid_checker.h"

#include <regex>

UcidChecker::UcidChecker() {}

UcidChecker::~UcidChecker() {}

bool UcidChecker::operator()(const std::string& str) const {
  std::regex pattern(R"(\d{32})");
  return std::regex_match(str, pattern);
}
