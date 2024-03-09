#ifndef RETURNSTATUSHANDLERBUNDLE_H
#define RETURNSTATUSHANDLERBUNDLE_H

#include <QObject>

#include "types.h"

class ReturnStatusHandlerBundle final : public QObject {
  Q_OBJECT
 public:
  explicit ReturnStatusHandlerBundle();
  ~ReturnStatusHandlerBundle() = default;

 private:
  Q_DISABLE_COPY_MOVE(ReturnStatusHandlerBundle)

 public:
  void handle(ReturnStatus ret);

 signals:
  void handle_signal(ReturnStatus ret);
};

#endif // RETURNSTATUSHANDLERBUNDLE_H
