#ifndef LOGSYSTEMCONNECTION_H
#define LOGSYSTEMCONNECTION_H

#include <QObject>

class LoggableObject;

class LogSystemConnection final : public QObject {
  Q_OBJECT
 public:
  explicit LogSystemConnection();
  ~LogSystemConnection() = default;

 private:
  Q_DISABLE_COPY_MOVE(LogSystemConnection)

 signals:
  void logging(const QString& log, const LoggableObject* source) const;
};

#endif  // LOGSYSTEMCONNECTION_H
