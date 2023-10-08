#ifndef GUI_H
#define GUI_H

#include <QApplication>
#include <QDesktopWidget>
#include <QObject>
#include <QtWidgets>

class GUI : public QWidget {
  Q_OBJECT
 public:
  enum GUI_Type { InitialConfiguration, Master, Production };
  Q_ENUM(GUI_Type)

 protected:
  GUI_Type Type;

 public:
  QHBoxLayout* MainLayout;

 public:
  explicit GUI(QWidget* parent, GUI_Type type);
  virtual ~GUI();

  virtual void create(void) = 0;
  virtual void update(void) = 0;

  GUI_Type type(void);

 private:
  Q_DISABLE_COPY(GUI)

 signals:
};

#endif  // GUI_H
