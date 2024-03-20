#include "pan_checker.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>

bool PanChecker::operator()(const QString& str) const {
  static QRegularExpression regex(R"(\d{19})");
  QRegularExpressionMatch match = regex.match(str);
  return match.hasMatch();
}
