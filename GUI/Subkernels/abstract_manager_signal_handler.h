#ifndef ABSTRACTMANAGERSIGNALHANDLER_H
#define ABSTRACTMANAGERSIGNALHANDLER_H

#include <QElapsedTimer>
#include <QProgressDialog>
#include <QTimer>

#include "abstract_gui_subkernel.h"
#include "types.h"

class AbstractManagerSignalHandler : public AbstractGuiSubkernel {
  Q_OBJECT
 private:
  std::unique_ptr<QProgressDialog> ProgressDialog;

  std::unique_ptr<QTimer> ODTimer;
  std::unique_ptr<QTimer> ODQTimer;
  std::unique_ptr<QElapsedTimer> ODMeter;

  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  explicit AbstractManagerSignalHandler(const QString& name);
  virtual ~AbstractManagerSignalHandler();

  // Own
 public slots:
  virtual void processOperationStart(const QString& operationName);
  virtual void processOperationFinish(const QString& operationName,
                                      ReturnStatus ret);

 private:
  Q_DISABLE_COPY_MOVE(AbstractManagerSignalHandler)
  virtual void loadSettings(void) override;

  void createProgressDialog(void);
  void destroyProgressDialog(void);
  void createTimers(void);

  void createMessageMatchTable(void);
  void processReturnStatus(ReturnStatus ret);

 private slots:
  void progressDialogCanceled_slot(void);

  void ODTimerTimeout_slot(void);
  void ODQTimerTimeout_slot(void);

 signals:
  void abortCurrentOperation(void);
};

#endif  // ABSTRACTMANAGERSIGNALHANDLER_H
