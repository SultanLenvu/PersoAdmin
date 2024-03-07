#ifndef MEASURING_PROGRESS_INDICATOR_H
#define MEASURING_PROGRESS_INDICATOR_H

#include <QElapsedTimer>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QTimer>

#include "abstract_progress_indicator.h"
#include "loggable_object.h"

class MeasuringProgressIndicator final : public AbstractProgressIndicator,
                                         public LoggableObject {
  Q_OBJECT

 private:
  std::unique_ptr<QProgressDialog> ProgressDialog;

  std::unique_ptr<QTimer> ODTimer;
  std::unique_ptr<QTimer> ODQTimer;
  std::unique_ptr<QElapsedTimer> ODMeter;

 public:
  MeasuringProgressIndicator(const QString& name);
  ~MeasuringProgressIndicator();

  // AbstractProgressIndicator interface
 public slots:
  virtual void begin(const QString& operationName) override;
  virtual void finish(const QString& operationName) override;

 private:
  Q_DISABLE_COPY_MOVE(MeasuringProgressIndicator)

  void createProgressDialog(void);
  void destroyProgressDialog(void);
  void createTimers(void);

 private slots:
  void progressDialogCanceled_slot(void);

  void ODTimerTimeout_slot(void);
  void ODQTimerTimeout_slot(void);
};

#endif  // MEASURING_PROGRESS_INDICATOR_H
