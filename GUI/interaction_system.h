#ifndef INTERACTION_SYSTEM_H
#define INTERACTION_SYSTEM_H

#include <QElapsedTimer>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QTimer>

#include "types.h"

class InteractionSystem : public QWidget {
  Q_OBJECT

 private:
  std::unique_ptr<QProgressDialog> ProgressDialog;

  std::unique_ptr<QTimer> ODTimer;
  std::unique_ptr<QTimer> ODQTimer;
  std::unique_ptr<QElapsedTimer> ODMeter;

  std::unordered_map<ReturnStatus, QString> MessageTable;

 public:
  InteractionSystem(const QString& name);
  ~InteractionSystem();

 public slots:
  void generateMessage(const QString& text);
  void generateErrorMessage(const QString& text);

  void processOperationStart(const QString& operationName);
  void processOperationFinish(const QString& operationName, ReturnStatus ret);

  void applySettings(void);

 private:
  InteractionSystem();
  Q_DISABLE_COPY_MOVE(InteractionSystem)

 private:
  void loadSettings(void);
  void sendLog(const QString& log);

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
  void logging(const QString& log);
  void abortCurrentOperation(void);
};

#endif  // INTERACTION_SYSTEM_H
