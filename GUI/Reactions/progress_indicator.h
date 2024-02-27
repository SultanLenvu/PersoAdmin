#ifndef PROGRESS_INDICATOR_H
#define PROGRESS_INDICATOR_H

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
#include "types.h"

class ProgressIndicator final : public AbstractProgressIndicator {
  Q_OBJECT

 private:
  std::unique_ptr<QProgressDialog> ProgressDialog;

  std::unique_ptr<QTimer> ODTimer;
  std::unique_ptr<QTimer> ODQTimer;
  std::unique_ptr<QElapsedTimer> ODMeter;

  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  ProgressIndicator(const QString& name);
  ~ProgressIndicator();

  // AbstractProgressIndicator interface
 public slots:
  virtual void begin(const QString& operationName) override;
  virtual void finish(const QString& operationName) override;

 private:
  Q_DISABLE_COPY_MOVE(ProgressIndicator)

  void createProgressDialog(void);
  void destroyProgressDialog(void);
  void createTimers(void);

  void createMessageMatchTable(void);
  void processReturnStatus(ReturnStatus ret);

 private slots:
  void progressDialogCanceled_slot(void);

  void ODTimerTimeout_slot(void);
  void ODQTimerTimeout_slot(void);
};

#endif  // PROGRESS_INDICATOR_H
