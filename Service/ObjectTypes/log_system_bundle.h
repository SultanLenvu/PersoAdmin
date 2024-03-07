#ifndef LOGSYSTEMCONNECTION_H
#define LOGSYSTEMCONNECTION_H

#include <QObject>

class LogSystemBundle final : public QObject {
  Q_OBJECT
 public:
  explicit LogSystemBundle();
  ~LogSystemBundle() = default;

 public:
  void send(const QString& msg) const;

 private:
  Q_DISABLE_COPY_MOVE(LogSystemBundle)

 signals:
  void logging(const QString& log) const;
};

#endif  // LOGSYSTEMCONNECTION_H
