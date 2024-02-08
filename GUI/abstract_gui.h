#ifndef ABSTRACT_GUI_H
#define ABSTRACT_GUI_H

#include <QApplication>
#include <QObject>
#include <QtWidgets>

class AbstractGui : public QWidget {
  Q_OBJECT
 public:
  enum GuiType { InitialConfiguration, Master, Production };
  Q_ENUM(GuiType)

 protected:
  GuiType Type;

 public:
  QHBoxLayout* MainLayout;

 public:
  explicit AbstractGui(QWidget* parent, GuiType type);
  virtual ~AbstractGui();

  virtual void create(void) = 0;
  virtual void update(void) = 0;

  GuiType type(void);

 private:
  Q_DISABLE_COPY_MOVE(AbstractGui)

 signals:
};

#endif  // ABSTRACT_GUI_H
