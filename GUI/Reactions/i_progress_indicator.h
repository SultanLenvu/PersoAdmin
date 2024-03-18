#ifndef IPROGRESSINDICATOR_H
#define IPROGRESSINDICATOR_H

#include <QString>

class IProgressIndicator {
 public:
  explicit IProgressIndicator(void) = default;
  virtual ~IProgressIndicator(void) = default;

 public:
  virtual void begin(const QString& opname) = 0;
  virtual void finish(const QString& opname) = 0;
};

#endif  // IPROGRESSINDICATOR_H
