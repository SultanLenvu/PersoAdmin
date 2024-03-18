#ifndef STATUSINDICATORLINK_H
#define STATUSINDICATORLINK_H

#include <QObject>

#include "types.h"

class StatusIndicatorLink final : public QObject {
  Q_OBJECT
 public:
  explicit StatusIndicatorLink();
  ~StatusIndicatorLink() = default;

 private:
  Q_DISABLE_COPY_MOVE(StatusIndicatorLink)

 signals:
  void handle(ReturnStatus ret);
};

#endif  // STATUSINDICATORLINK_H
