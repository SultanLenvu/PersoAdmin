#ifndef PROGRESSINDICATORLINK_H
#define PROGRESSINDICATORLINK_H

#include <QObject>

class ProgressIndicatorLink final : public QObject {
  Q_OBJECT
 public:
  explicit ProgressIndicatorLink();
  ~ProgressIndicatorLink() = default;

 private:
  Q_DISABLE_COPY_MOVE(ProgressIndicatorLink)

 signals:
  void begin(const QString& name);
  void finish(const QString& name);
};

#endif  // PROGRESSINDICATORLINK_H
