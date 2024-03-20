#include "numeric_string_checker.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

bool NumericStringChecker::operator()(const QString& str) const {
  static QRegularExpression regex(R"("[0-9]+")");
  QRegularExpressionMatch match = regex.match(str);
  return match.hasMatch();
}
