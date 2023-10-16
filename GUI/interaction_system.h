#ifndef INTERACTION_SYSTEM_H
#define INTERACTION_SYSTEM_H

#include <QDebug>
#include <QElapsedTimer>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QObject>
#include <QProgressBar>
#include <QProgressDialog>
#include <QSettings>
#include <QTimer>

#include "General/definitions.h"

class InteractionSystem : public QWidget {
  Q_OBJECT

 private:
  bool LogEnable;
  QProgressDialog* ProgressDialog;
  uint32_t CurrentOperationStep;

  QTimer* ODTimer;
  QTimer* ODQTimer;
  QElapsedTimer* ODMeter;

 public:
  static InteractionSystem* instance(void);

 public slots:
  void generateMessage(const QString& pass);
  void getMasterPassword(QString& pass);
  void generateErrorMessage(const QString& text);

  void startOperationProgressDialog(const QString& operationName);
  void finishOperationProgressDialog(const QString& operationName);

  void applySettings(void);

 private:
  explicit InteractionSystem(QWidget* window);
  Q_DISABLE_COPY(InteractionSystem)
  void loadSettings(void);
  void sendLog(const QString& log);

  void createProgressDialog(void);
  void destroyProgressDialog(void);
  void createTimers(void);

 private slots:
  void on_ProgressDialogCanceled_slot(void);

  void on_ODTimerTimeout_slot(void);
  void on_ODQTimerTimeout_slot(void);

 signals:
  void logging(const QString& log);
  void abortCurrentOperation(void);
};

#endif  // INTERACTION_SYSTEM_H
