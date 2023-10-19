#ifndef UNIFIEDDIALOGDESIGNER_H
#define UNIFIEDDIALOGDESIGNER_H

#include <QDialog>
#include <QList>
#include <QtWidgets>

class UnifiedDialogDesigner : public QDialog {
  Q_OBJECT
 private:
  QRect DesktopGeometry;

  QGridLayout* MainLayout;
  QHash<QString, QString> InputData;
  QHash<QString, QWidget*> InputWidgets;

 public:
  explicit UnifiedDialogDesigner(QWidget* parent);
  ~UnifiedDialogDesigner();

  void createPalletShippingDialog();
  const QHash<QString, QString>* getInputData(void);

  // QDialog interface
 public slots:
  virtual void accept() override;
  virtual void reject() override;

 private:
  Q_DISABLE_COPY(UnifiedDialogDesigner);
  void clear();
};

#endif // UNIFIEDDIALOGDESIGNER_H
