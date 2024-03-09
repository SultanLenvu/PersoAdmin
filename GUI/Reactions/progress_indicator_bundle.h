#ifndef PROGRESSINDICATORBUNDLE_H
#define PROGRESSINDICATORBUNDLE_H

#include <QObject>

class ProgressIndicatorBundle final : public QObject {
  Q_OBJECT
 public:
  explicit ProgressIndicatorBundle();
  ~ProgressIndicatorBundle() = default;

 public:
  void begin(const QString& name);
  void finish(const QString& name);

 private:
  Q_DISABLE_COPY_MOVE(ProgressIndicatorBundle)

 signals:
  void executionStarted(const QString& name);
  void executionFinished(const QString& name);
};

#endif // PROGRESSINDICATORBUNDLE_H
