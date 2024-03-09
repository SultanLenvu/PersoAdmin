#ifndef LOGSYSTEMCONNECTION_H
#define LOGSYSTEMCONNECTION_H

#include <QObject>

class LoggableObject;

class LogSystemBundle final : public QObject {
  Q_OBJECT
 public:
  explicit LogSystemBundle();
  ~LogSystemBundle() = default;

 private:
  Q_DISABLE_COPY_MOVE(LogSystemBundle)

 signals:
  void logging(const QString& log, const LoggableObject* source) const;
};

#endif  // LOGSYSTEMCONNECTION_H
