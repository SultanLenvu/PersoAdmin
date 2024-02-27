#ifndef ABSTRACTCHECKER_H
#define ABSTRACTCHECKER_H

#include <string>

class AbstractStringChecker {
 public:
  AbstractStringChecker();
  virtual ~AbstractStringChecker();

 public:
  virtual bool operator()(const std::string&) const = 0;
};

#endif // ABSTRACTCHECKER_H
