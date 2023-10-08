#ifndef USER_INTERACTION_SYSTEM_H
#define USER_INTERACTION_SYSTEM_H

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

class UserInteractionSystem : public QObject {
  Q_OBJECT

 private:
  bool LogEnable;
  QWidget* ParentWindow;
  QProgressDialog* ProgressDialog;
  uint32_t CurrentOperationStep;

  QTimer* ODTimer;
  QTimer* ODQTimer;
  QElapsedTimer* ODMeter;

 public:
  explicit UserInteractionSystem(QWidget* window);

 public slots:
  void generateNotification(const QString& pass);
  void getMasterPassword(QString& pass);
  void generateError(const QString& text);

  void startOperationProgressDialog(const QString& operationName);
  void finishOperationProgressDialog(const QString& operationName);

  void applySettings(void);

 private:
  Q_DISABLE_COPY(UserInteractionSystem)
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

#endif  // USER_INTERACTION_SYSTEM_H
